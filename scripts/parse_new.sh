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

mkdir -p "${HOME}/logs/"
LOGFILE="${HOME}/logs/parse_new.${HOST}.${PROVIDER_LC}.${YESTERDAY}.log"
touch "$LOGFILE"
exec 1>"$LOGFILE"
exec 2>&1

echo "YESTERDAY=$YESTERDAY YESTERDAY_UNDER=$YESTERDAY_UNDER YESTERDAY_DASH=$YESTERDAY_DASH"

if [[ ${#YESTERDAY_UNDER} -ne 10 ]]; then
    echo "Invalid date: $YESTERDAY_UNDER"
    echo "$USAGE"
    exit 2
fi

mkdir -p "${HOME}/done/"
DONEFILE="${HOME}/done/${PROVIDER}_${YESTERDAY}.done"
if [ -e "$DONEFILE" ]; then
    echo "$DONEFILE done"
    exit 0
else
    touch "$DONEFILE"
fi

buckets=$(sqlcmd "select distinct log_bucket from buckets where cloud_provider='$PROVIDER' and scope='${STRIDES_SCOPE}' order by log_bucket")
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
ls -l "$PARSER_BIN"

for LOG_BUCKET in "${buckets[@]}"; do
    BUCKET_NAME=$(sqlcmd "select distinct bucket_name from buckets where cloud_provider='$PROVIDER' and log_bucket='$LOG_BUCKET' limit 1")
    if [ -n "$BUCKET_ONLY" ]; then
        if [ "$BUCKET_NAME" != "$BUCKET_ONLY" ]; then
            echo "Skipping $BUCKET_NAME != $BUCKET_ONLY"
            continue
        fi
    fi

    echo "  Parsing $LOG_BUCKET..."
    echo "    BUCKET_NAME is $BUCKET_NAME"

    PARSER_REC=$(sqlcmd "select parser_binary || ':' || parser_options from log_formats where log_format = (select log_format from buckets where cloud_provider='$PROVIDER' and bucket_name='$BUCKET_NAME')")
    PARSER_BIN=$(echo "$PARSER_REC" | cut -d':' -f 1)
    PARSER_OPT=$(echo "$PARSER_REC" | cut -d':' -f 2)

    echo "    PARSER_BIN is '$PARSER_BIN', PARSER_OPT is '$PARSER_OPT'"
    if [ "$PROVIDER" = "OP" ]; then
        #        if [[ "$LOG_BUCKET" =~ "srafiles" ]]; then
        #            BUCKET_NAME="srafiles"
        #        fi
        LOG_BUCKET="OP-${BUCKET_NAME}"
        if [ "$LOG_BUCKET" == "OP-OP" ]; then
            LOG_BUCKET="OP"
        fi
    fi

    PARSE_DEST="$TMP/parsed/$PROVIDER/$LOG_BUCKET/$YESTERDAY"
    mkdir -p "$PARSE_DEST"
    cd "$PARSE_DEST" || exit
    df -HT . | indent

    if [ "$STRIDES_SCOPE" = "public" ]; then
        SRC_BUCKET="gs://logmon_logs/${PROVIDER_LC}_${STRIDES_SCOPE}/"
    fi

    if [ "$STRIDES_SCOPE" = "private" ]; then
        SRC_BUCKET="gs://logmon_logs_private/${PROVIDER_LC}_${STRIDES_SCOPE}/"
        export GOOGLE_APPLICATION_CREDENTIALS="$HOME/logmon_private.json"
    fi

    TGZ="$YESTERDAY_DASH.$LOG_BUCKET.tar.gz"
    echo "  Copying $TGZ to $PARSE_DEST"

    export CLOUDSDK_CORE_PROJECT="ncbi-logmon"
    gcloud config set account 253716305623-compute@developer.gserviceaccount.com

    gsutil -o 'GSUtil:sliced_object_download_threshold=0' cp "${SRC_BUCKET}${TGZ}" . || true
    if [ ! -e "$TGZ" ]; then
        echo "  ${SRC_BUCKET}${TGZ} not found, skipping"
        continue
    fi
    ls -hl "$TGZ"
    tarsz=$(stat -c %s "$TGZ")
    if [ "$tarsz" -lt 100 ]; then
        echo " $TGZ too small ($tarsz bytes), skipping"
        continue
    fi

    echo "  Counting $TGZ ..."
    totalwc=$(tar -xaOf "$TGZ" "$WILDCARD" | wc -l | cut -f1 -d' ') || true
    #touch "$YESTERDAY_DASH.${LOG_BUCKET}.json"

    if [ "$totalwc" -gt 0 ]; then
        VERSION=$($PARSER_BIN --version)
        echo "  Parsing $TGZ (pattern=$WILDCARD), $totalwc lines with $PARSER_BIN $VERSION..."

        BASE="$LOG_BUCKET.$YESTERDAY_DASH"
        echo "    BASE is $BASE"

        mkfifo "$BASE.good.jsonl"

        # shellcheck disable=SC2016
        split -a 3 -d -l 10000000 \
            --filter='gzip -9 > $FILE.jsonl.gz' \
            - "recognized.$BASE."  \
            < "$BASE.good.jsonl" &

        set +e
        tar -xaOf "$TGZ" "$WILDCARD" | \
            time "$PARSER_BIN" -f -t 2 "$BASE" \
            > stdout."$BASE" \
            2> stderr."$BASE"

        echo "    Returned $?"
        rm -f "$TGZ"
        head -v stderr."$BASE"
        echo "==="

        ls -l

        echo

        echo "  Record format is:"
        zcat recognized."$BASE".*.jsonl.gz | head -1 | jq -SM .


        touch "$BASE.bad.jsonl"
        touch "$BASE.review.jsonl"
        touch "$BASE.ugly.jsonl"
        touch "$BASE.unrecog.jsonl"

        cat "$BASE.unrecog.jsonl" \
            "$BASE.bad.jsonl" \
            "$BASE.review.jsonl" \
            "$BASE.ugly.jsonl" \
            > "unrecognized.$BASE.jsonl"

        rm -f "$BASE.unrecog.jsonl" "$BASE.bad.jsonl" "$BASE.review.jsonl" \
            "$BASE.ugly.jsonl" "$BASE.good.jsonl"

        mv "$BASE.stats.jsonl" "stats.$BASE.jsonl"

        # shellcheck disable=SC2016
        recwc=$(zcat recognized."$BASE".*.jsonl.gz | wc -l | cut -f1 -d' ')
        printf "Recognized lines:   %8d\n" "$recwc"
        unrecwc=$(wc -l "unrecognized.$BASE.jsonl" | cut -f1 -d' ')
        printf "Unrecognized lines: %8d\n" "$unrecwc"

        #if [ ! -s "recognized.$BASE.jsonl" ]; then
        #    echo "ERROR: Empty recognized.$BASE.jsonl"
        #    #continue
        #fi

        #if [ ! -s "unrecognized.$BASE.jsonl" ]; then
        #    rm -f "unrecognized.$BASE.jsonl"
        #fi

        echo "  Gzipping ..."
        gzip -f -v ./*recognized."$BASE"*jsonl
        find ./ -name "*$BASE*" -size 0c -exec rm -f {} \;

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
        } | jq -S -c . > "summary.$BASE.jsonl"
        echo "  Summary:"
        jq -M -S . < summary."$BASE".jsonl | indent

        ls -l
        if [ "$STRIDES_SCOPE" = "public" ]; then
            DEST_BUCKET="gs://logmon_logs_parsed_us"
            export GOOGLE_APPLICATION_CREDENTIALS=$HOME/logmon.json
            export CLOUDSDK_CORE_PROJECT="ncbi-logmon"
            gcloud config set account 253716305623-compute@developer.gserviceaccount.com
        fi

        if [ "$STRIDES_SCOPE" = "private" ]; then
            export GOOGLE_APPLICATION_CREDENTIALS="$HOME/logmon_private.json"
            DEST_BUCKET="gs://logmon_logs_parsed_private"
        fi

        echo "  Uploading to $DEST_BUCKET..."
        gsutil -q cp ./*ecognized."$BASE"* "$DEST_BUCKET/logs_${PROVIDER_LC}_${STRIDES_SCOPE}/v3/"
        gsutil -q cp ./std*."$BASE"* "$DEST_BUCKET/logs_${PROVIDER_LC}_${STRIDES_SCOPE}/v3/"
        gsutil -q cp ./stats."$BASE".jsonl "$DEST_BUCKET/logs_${PROVIDER_LC}_${STRIDES_SCOPE}/v3/"
        gsutil -q cp ./summary."$BASE".jsonl "$DEST_BUCKET/logs_${PROVIDER_LC}_${STRIDES_SCOPE}/v3/"

        gsutil ls -lh \
            "$DEST_BUCKET/logs_${PROVIDER_LC}_${STRIDES_SCOPE}/v3/*$BASE*" | \
            indent
    else
        echo "File has $totalwc records, skipping"
    fi # totalwc gt 0

cd ..
rm -rf "$PARSE_DEST"
echo "  Done $LOG_BUCKET for $YESTERDAY_DASH..."
echo
done

echo " Done parse_new"
date
