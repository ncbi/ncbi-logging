#!/bin/bash

USAGE="Usage: $0 {S3,GS,OP} YYYY_MM_DD"
if [ "$#" -ne 2 ]; then
    echo "$USAGE"
    exit 1
fi

PROVIDER=$1
DATE_UNDER=$2
PROVIDER_LC=${PROVIDER,,}

# shellcheck source=strides_env.sh
. ./strides_env.sh

case "$PROVIDER" in
    S3)
        PARSER="aws"
        ;;
    GS)
        PARSER="gcp"
        ;;
    OP)
        PARSER="op"
        ;;
    *)
        echo "Invalid provider $PROVIDER"
        echo "$USAGE"
        exit 1
esac

if [[ ${#DATE_UNDER} -ne 10 ]]; then
    echo "Invalid date: $DATE_UNDER"
    echo "$USAGE"
    exit 2
fi

DATE=${DATE_UNDER//_}
export DATE_DASH=${DATE_UNDER//_/-}

buckets=$(sqlcmd "select distinct log_bucket from buckets where cloud_provider='$PROVIDER' order by log_bucket desc")

echo "buckets is '$buckets'"
for LOG_BUCKET in $buckets; do
    echo "Parsing $LOG_BUCKET..."

    export GOOGLE_APPLICATION_CREDENTIALS=$HOME/sandbox-blast-847af7ab431a.json
    gcloud config set account 1008590670571-compute@developer.gserviceaccount.com
    export CLOUDSDK_CORE_PROJECT="ncbi-sandbox-blast"

    PARSE_DEST="$RAMDISK/$PROVIDER/$LOG_BUCKET"

    TGZ="$PROVIDER.$LOG_BUCKET.$DATE.tar.gz"

    mkdir -p "$PARSE_DEST"
    cd "$PARSE_DEST" || exit
    df -HT .

    echo "Copying $TGZ to $PARSE_DEST"
    gsutil cp "gs://strides_analytics_logs_${PROVIDER_LC}_public/$TGZ" "$TGZ"
    ls -l "$TGZ"

    echo "Counting $TGZ"
    totalwc=$(tar -xaOf "$TGZ" | wc -l | cut -f1 -d' ')
    echo "Parsing $TGZ, $totalwc lines"
    touch "$DATE.${LOG_BUCKET}.json"

    tar -xaOf "$TGZ" | \
        time "$HOME/devel/ncbi-logging/parser/bin/log2jsn-rel" "$PARSER" > \
        "$DATE.${LOG_BUCKET}.json" \
        2> "$TGZ.err"
    #newwc=$(wc -l "$DATE"."${LOG_BUCKET}".json | cut -f1 -d' ')

    head -v ./*.err
    echo

    set +e
    grep "{\"unrecognized\":\"" "$DATE.${LOG_BUCKET}.json" > \
        "unrecognized.$DATE.${LOG_BUCKET}.jsonl"
    grep -v "{\"unrecognized\":\"" "$DATE.${LOG_BUCKET}.json" > \
        "recognized.$DATE.${LOG_BUCKET}.jsonl"
    set -e

    unrecwc=$(wc -l "unrecognized.$DATE.${LOG_BUCKET}.jsonl" | cut -f1 -d' ')
    recwc=$(wc -l "recognized.$DATE.${LOG_BUCKET}.jsonl" | cut -f1 -d' ')

    printf "Recognized lines:   %8d\n" "$recwc"
    printf "Unrecognized lines: %8d\n" "$unrecwc"

    echo "Verifying JSON..."
    jq -e -c . < "recognized.$DATE.${LOG_BUCKET}.jsonl" > /dev/null
    jq -e -c . < "unrecognized.$DATE.${LOG_BUCKET}.jsonl" > /dev/null

    if [ "$unrecwc" -eq "0" ]; then
        echo "Gzipping..."
        find ./ -name "*.jsonl" -size 0c -exec rm -f {} \;  # Don't bother with empty
        gzip -v -9 ./*.jsonl

        ls -la

        echo "Uploading..."
        gsutil cp ./*.jsonl.gz "gs://strides_analytics_logs_parsed/$LOG_BUCKET/"
        gsutil ls -l "gs://${PREFIX}_logs_parsed/$LOG_BUCKET/"
        #rm -rf "$DEST"
    fi
done
