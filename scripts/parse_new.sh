#!/bin/bash

# Seed:
# rm -f days
# for d in $(seq 1 470); do
# D=$(date -d "-$d days" "+%Y_%m_%d")
# echo $D >> days
# done
# cat days | xargs -P 10 -I % ./parse.sh GS %

USAGE="Usage: $0 {S3,GS,OP,Splunk} [YYYY_MM_DD [bucket]]"

sleep $((RANDOM / 500))

# shellcheck source=strides_env.sh
. ./strides_env.sh


case "$#" in
    0)
        echo "$USAGE"
        exit 1
        ;;
    1)
        PROVIDER=$1
        YESTERDAY_UNDER=$(date -d "yesterday" "+%Y_%m_%d") #_%H%
        BUCKET_ONLY=""
        ;;
    2)
        PROVIDER=$1
        YESTERDAY_UNDER=$2
        BUCKET_ONLY=""
        ;;
    3)
        PROVIDER=$1
        YESTERDAY_UNDER=$2
        BUCKET_ONLY=$3
        ;;
    *)
        echo "$USAGE"
        exit 1
        ;;
esac

PROVIDER_LC=${PROVIDER,,}
YESTERDAY=${YESTERDAY_UNDER//_}
YESTERDAY_DASH=${YESTERDAY_UNDER//_/-}

echo "YESTERDAY=$YESTERDAY YESTERDAY_UNDER=$YESTERDAY_UNDER YESTERDAY_DASH=$YESTERDAY_DASH"

if [[ ${#YESTERDAY_UNDER} -ne 10 ]]; then
    echo "Invalid date: $YESTERDAY_UNDER"
    echo "$USAGE"
    exit 2
fi

buckets=$(sqlcmd "select distinct log_bucket from buckets where cloud_provider='$PROVIDER' and scope='public' order by log_bucket")
readarray -t buckets <<< "$buckets"
echo "buckets has ${#buckets[@]}, is ' " "${buckets[*]}" "'"

case "$PROVIDER" in
    OP)
#        PARSER_BIN="op2jsn-rel"
        WILDCARD='*'
#        buckets=("OP") # TODO
        ;;
    GS)
#        PARSER_BIN="gcp2jsn-rel"
        WILDCARD='*'
        ;;
    S3)
#        PARSER_BIN="aws2jsn-rel"
        WILDCARD='*'
        ;;
    Splunk)
        WILDCARD='*'
        ;;
    *)
        echo "Invalid provider $PROVIDER"
        echo "$USAGE"
        exit 1
        ;;
esac

for LOG_BUCKET in "${buckets[@]}"; do
    BUCKET_NAME=$(sqlcmd "select distinct bucket_name from buckets where cloud_provider='$PROVIDER' and log_bucket='$LOG_BUCKET' limit 1")
    if [ -n "$BUCKET_ONLY" ]; then
        if [ "$BUCKET_NAME" != "$BUCKET_ONLY" ]; then
            echo "Skipping $BUCKET_NAME != $BUCKET_ONLY"
            continue
        fi
    fi

    echo "  Parsing $LOG_BUCKET..."
    echo "BUCKET_NAME is $BUCKET_NAME"

    PARSER_REC=$(sqlcmd "select parser_binary || ':' || parser_options from log_formats where log_format = (select log_format from buckets where cloud_provider='$PROVIDER' and bucket_name='$BUCKET_NAME')")
    PARSER_BIN=$(echo "$PARSER_REC" | cut -d':' -f 1)
    PARSER_OPT=$(echo "$PARSER_REC" | cut -d':' -f 2)

    echo "PARSER_BIN is '$PARSER_BIN', PARSER_OPT is '$PARSER_OPT'"
    if [ "$PROVIDER" = "OP" ]; then
#        if [[ "$LOG_BUCKET" =~ "srafiles" ]]; then
#            BUCKET_NAME="srafiles"
#        fi
        LOG_BUCKET="OP-${BUCKET_NAME}"
    fi

    PARSE_DEST="$TMP/parsed/$PROVIDER/$LOG_BUCKET/$YESTERDAY"
    mkdir -p "$PARSE_DEST"
    cd "$PARSE_DEST" || exit
    df -HT .

    SRC_BUCKET="gs://logmon_logs/${PROVIDER_LC}_public/"
    TGZ="$YESTERDAY_DASH.$LOG_BUCKET.tar.gz"
    echo "  Copying $TGZ to $PARSE_DEST"

    export CLOUDSDK_CORE_PROJECT="ncbi-logmon"
    gcloud config set account 253716305623-compute@developer.gserviceaccount.com
    gsutil -o 'GSUtil:sliced_object_download_threshold=0' cp "${SRC_BUCKET}${TGZ}" .
    ls -hl "$TGZ"

    echo "  Counting $TGZ ..."
    totalwc=$(tar -xaOf "$TGZ" "$WILDCARD" | wc -l | cut -f1 -d' ')
    #touch "$YESTERDAY_DASH.${LOG_BUCKET}.json"

    VERSION=$($PARSER_BIN --version)
    echo "  Parsing $TGZ (pattern=$WILDCARD), $totalwc lines with $PARSER_BIN $VERSION..."

    BASE="$LOG_BUCKET.$YESTERDAY_DASH"
    echo "BASE is $BASE"

    set +e
    if [ "$PROVIDER" = "Splunk" ]; then
        # Remove when LOGMON-107 fixed
        tar -xaOf "$TGZ" "$WILDCARD" | \
            tr -s ' ' | \
            time "$PARSER_BIN" -f -t 4 "$BASE" \
            > stdout."$BASE" \
            2> stderr."$BASE"
    else
        tar -xaOf "$TGZ" "$WILDCARD" | \
            time "$PARSER_BIN" -f -t 4 "$BASE" \
            > stdout."$BASE" \
            2> stderr."$BASE"
    fi

    echo "Returned $?"
    rm -f "$TGZ"
    head -v stderr."$BASE"
    echo "==="

    ls -l

    echo

    echo "  Record format is:"
    head -1 "$BASE.good.jsonl" | jq -SM .


    touch "$BASE.bad.jsonl"
    touch "$BASE.good.jsonl"
    touch "$BASE.review.jsonl"
    touch "$BASE.ugly.jsonl"
    touch "$BASE.unrecog.jsonl"

    cat "$BASE.unrecog.jsonl" \
        "$BASE.bad.jsonl" \
        "$BASE.review.jsonl" \
        "$BASE.ugly.jsonl" \
        > "unrecognized.$BASE.jsonl"
    rm -f "$BASE.unrecog.jsonl" "$BASE.bad.jsonl" "$BASE.review.jsonl" "$BASE.ugly.jsonl"
    mv "$BASE.good.jsonl" "recognized.$BASE.jsonl"
    mv "$BASE.stats.jsonl" "stats.$BASE.jsonl"

    recwc=$(wc -l "recognized.$BASE.jsonl" | cut -f1 -d' ')
    printf "Recognized lines:   %8d\n" "$recwc"
    unrecwc=$(wc -l "unrecognized.$BASE.jsonl" | cut -f1 -d' ')
    printf "Unrecognized lines: %8d\n" "$unrecwc"

    if [ ! -s "recognized.$BASE.jsonl" ]; then
        echo "ERROR: Empty recognized.$BASE.jsonl"
        #continue
    fi

    #if [ ! -s "unrecognized.$BASE.jsonl" ]; then
    #    rm -f "unrecognized.$BASE.jsonl"
    #fi

    echo
    echo "  Splitting ..."
    # -e no empty
    split -a 3 -d -l 10000000 --additional-suffix=.jsonl \
        - "recognized.$BASE." \
        < "recognized.$BASE.jsonl"

    rm -f "recognized.$BASE.jsonl"

    echo "  Gzipping ..."
    gzip -f -v ./*recognized."$BASE"*jsonl
    find ./ -name "*$BASE*" -size 0c -exec rm -f {} \;

    echo -n "  Summarizing... "
    {
        printf "{"
        printf '"log_bucket": "%s",' "$LOG_BUCKET"
        printf '"provider": "%s",' "$PROVIDER"
        printf '"log_date": "%s",' "$YESTERDAY"
        printf '"parse_date": "%s",' "$DATE"
        printf '"parser_version" : "%s",' "$VERSION"
        printf '"total_lines" : %d,' "$totalwc"
        printf '"recognized_lines" : %d,' "$recwc"
        printf '"unrecognized_lines" : %d'  "$unrecwc"
        printf "}"
    } | jq -S -c . | tee "summary.$BASE.jsonl"

    ls -l
    echo "  Uploading..."

    export GOOGLE_APPLICATION_CREDENTIALS=$HOME/logmon.json
    export CLOUDSDK_CORE_PROJECT="ncbi-logmon"
    gcloud config set account 253716305623-compute@developer.gserviceaccount.com
    gsutil cp ./*ecognized."$BASE"* "gs://logmon_logs_parsed_us/logs_${PROVIDER_LC}_public/v3/"
    gsutil cp ./std*."$BASE"* "gs://logmon_logs_parsed_us/logs_${PROVIDER_LC}_public/v3/"
    gsutil cp ./stats."$BASE".jsonl "gs://logmon_logs_parsed_us/logs_${PROVIDER_LC}_public/v3/"
    gsutil cp ./summary."$BASE".jsonl "gs://logmon_logs_parsed_us/logs_${PROVIDER_LC}_public/v3/"

    gsutil ls -lh "gs://logmon_logs_parsed_us/logs_${PROVIDER_LC}_public/v3/*$BASE*"

    cd ..
    rm -rf "$PARSE_DEST"
echo "  Done $LOG_BUCKET for $YESTERDAY_DASH..."
echo
done
