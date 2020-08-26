#!/bin/bash

# Seed:
# rm -f days
# for d in $(seq 1 470); do
# D=$(date -d "-$d days" "+%Y_%m_%d")
# echo $D >> days
# done
# cat days | xargs -P 10 -I % ./parse.sh GS %

USAGE="Usage: $0 {S3,GS,OP} YYYY_MM_DD"

#sleep $((RANDOM / 500))

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
        ;;
    2)
        PROVIDER=$1
        YESTERDAY_UNDER=$2
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
        PARSER_BIN="op2jsn-rel"
        #WILDCARD='*access*'
        WILDCARD='*'
#        buckets=("OP") # TODO
        ;;
    GS)
        PARSER_BIN="gcp2jsn-rel"
        WILDCARD='*'
        ;;
    S3)
        PARSER_BIN="aws2jsn-rel"
        WILDCARD='*'
        ;;
    *)
        echo "Invalid provider $PROVIDER"
        echo "$USAGE"
        exit 1
        ;;
esac

for LOG_BUCKET in "${buckets[@]}"; do
    echo "  Parsing $LOG_BUCKET..."
    BUCKET_NAME=$(sqlcmd "select distinct bucket_name from buckets where cloud_provider='$PROVIDER' and log_bucket='$LOG_BUCKET' limit 1")
    echo "BUCKET_NAME is $BUCKET_NAME"

    if [ "$PROVIDER" = "OP" ]; then
        if [[ "$LOG_BUCKET" =~ "srafiles" ]]; then
            BUCKET_NAME="srafiles"
        fi
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
    totalwc=$(tar -xaOf "$TGZ" | wc -l | cut -f1 -d' ')
    touch "$YESTERDAY_DASH.${LOG_BUCKET}.json"

    VERSION=$($PARSER_BIN --version)
    echo "  Parsing $TGZ (pattern=$WILDCARD), $totalwc lines with $PARSER_BIN $VERSION..."

    BASE="$LOG_BUCKET.$YESTERDAY_DASH"
    tar -xaOf "$TGZ" "$WILDCARD" | \
        time "$PARSER_BIN" -f -t 4 "$BASE" \
        2> "$BASE.stderr"
    rm -f "$TGZ"
    head "$BASE.stderr"
#    touch "$BASE.ugly" # TODO

    ls -l

#    mv "$BASE.good" "$BASE.good.jsonl"
#    mv "$BASE.bad" "$BASE.bad.jsonl"
#    mv "$BASE.review" "$BASE.review.jsonl"
#    mv "$BASE.stderr" "$BASE.stderr.jsonl"
    echo

    echo "  Record format is:"
    head -1 "$BASE.good.jsonl" | jq -SM .

    set +e
    grep "\"accepted\":false," "$BASE.good.jsonl" > \
        "$BASE.unrecognized.jsonl"
    grep "\"accepted\":true," "$BASE.good.jsonl" > \
        "$BASE.recognized.jsonl"
    rm -f "$BASE.good"
    set -e

    # "accepted": true,
    unrecwc=$(wc -l "$BASE.unrecognized.jsonl" | cut -f1 -d' ')
    recwc=$(wc -l "$BASE.recognized.jsonl" | cut -f1 -d' ')

    printf "Recognized lines:   %8d\n" "$recwc"
    printf "Unrecognized lines: %8d\n" "$unrecwc"

    if [ ! -s "unrecognized.$BASE.jsonl" ]; then
        rm -f "unrecognized.$BASE.jsonl"
    fi

    echo "  splitting"
    split -a 3 -d -e -l 10000000 --additional-suffix=.jsonl \
        - "recognized.$BASE" \
        < "$BASE.recognized.jsonl"

    rm -f "recognized.$BASE.jsonl"
#        fi

    ls -l
    echo "  Gzipping $BASE..."
    gzip -f -v -9 ./"*recognized.$BASE.jsonl"

    ls -l

    echo "  Summarizing..."
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
    } > "$TGZ.json"
    jq -S -c . < "$TGZ.json" > "summary.$TGZ.jsonl"
    cat "summary.$TGZ.jsonl"

    ls -l
    echo "  Uploading..."

    export GOOGLE_APPLICATION_CREDENTIALS=$HOME/logmon.json
    export CLOUDSDK_CORE_PROJECT="ncbi-logmon"
    gcloud config set account 253716305623-compute@developer.gserviceaccount.com
    gsutil cp ./*ecognized."$BASE"*.jsonl.gz "gs://logmon_logs_parsed_us/logs_${PROVIDER_LC}_public/"
    gsutil cp ./"$TGZ.err" "gs://logmon_logs_parsed_us/logs_${PROVIDER_LC}_public/"
    gsutil cp ./"summary.$TGZ.jsonl" "gs://logmon_logs_parsed_us/logs_${PROVIDER_LC}_public/"
    cd ..
    rm -rf "$PARSE_DEST"
echo "  Done $LOG_BUCKET for $YESTERDAY_DASH..."
echo
done
