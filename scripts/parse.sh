#!/bin/bash

USAGE="Usage: $0 {S3,GS,OP} YYYY_MM_DD"

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

case "$PROVIDER" in
    S3)
        export PARSER="aws"
        ;;
    GS)
        export PARSER="gcp"
        ;;
    OP)
        export PARSER="op"
        ;;
    *)
        echo "Invalid provider $PROVIDER"
        echo "$USAGE"
        exit 1
esac

if [[ ${#YESTERDAY_UNDER} -ne 10 ]]; then
    echo "Invalid date: $YESTERDAY_UNDER"
    echo "$USAGE"
    exit 2
fi

buckets=$(sqlcmd "select distinct log_bucket from buckets where cloud_provider='$PROVIDER' order by log_bucket desc")

echo "buckets is '$buckets'"
for LOG_BUCKET in $buckets; do
    echo "Parsing $LOG_BUCKET..."

    export GOOGLE_APPLICATION_CREDENTIALS=$HOME/sandbox-blast-847af7ab431a.json
    gcloud config set account 1008590670571-compute@developer.gserviceaccount.com
    export CLOUDSDK_CORE_PROJECT="ncbi-sandbox-blast"

    PARSE_DEST="$TMP/$PROVIDER/$LOG_BUCKET"

    TGZ="$PROVIDER.$LOG_BUCKET.$YESTERDAY.tar.gz"

    mkdir -p "$PARSE_DEST"
    cd "$PARSE_DEST" || exit
    df -HT .

    echo "Copying $TGZ to $PARSE_DEST"
    gsutil cp "gs://strides_analytics_logs_${PROVIDER_LC}_public/$TGZ" "$TGZ"
    ls -l "$TGZ"

    echo "Counting $TGZ"
    totalwc=$(tar -xaOf "$TGZ" | wc -l | cut -f1 -d' ')
    echo "Parsing $TGZ, $totalwc lines"
    touch "$YESTERDAY.${LOG_BUCKET}.json"

    tar -xaOf "$TGZ" | \
        time "$HOME/devel/ncbi-logging/parser/bin/log2jsn-rel" "$PARSER" > \
        "$YESTERDAY.${LOG_BUCKET}.json" \
        2> "$TGZ.err"
    #newwc=$(wc -l "$YESTERDAY"."${LOG_BUCKET}".json | cut -f1 -d' ')
    head "$TGZ.err"

    echo

    set +e
    grep "\"accepted\":false," "$YESTERDAY.${LOG_BUCKET}.json" > \
        "unrecognized.$YESTERDAY.${LOG_BUCKET}.jsonl"
    grep "\"accepted\":true," "$YESTERDAY.${LOG_BUCKET}.json" > \
        "recognized.$YESTERDAY.${LOG_BUCKET}.jsonl"
    set -e

    # "accepted": true,
    unrecwc=$(wc -l "unrecognized.$YESTERDAY.${LOG_BUCKET}.jsonl" | cut -f1 -d' ')
    recwc=$(wc -l "recognized.$YESTERDAY.${LOG_BUCKET}.jsonl" | cut -f1 -d' ')

    printf "Recognized lines:   %8d\n" "$recwc"
    printf "Unrecognized lines: %8d\n" "$unrecwc"

#    echo "Verifying JSON..."
#    jq -e -c . < "recognized.$YESTERDAY.${LOG_BUCKET}.jsonl" > /dev/null
#    jq -e -c . < "unrecognized.$YESTERDAY.${LOG_BUCKET}.jsonl" > /dev/null

    if [ "$unrecwc" -eq "0" ]; then
        #find ./ -name "*.jsonl" -size 0c -exec rm -f {} \;  # Don't bother with empty

        if [ "$recwc" -gt 10000000 ]; then
            echo "jsonl too large, splitting"
            split -d -e -l 10000000 --additional-suffix=.jsonl \
                - "recognized.$YESTERDAY.${LOG_BUCKET}." \
                < "recognized.$YESTERDAY.${LOG_BUCKET}.jsonl"

            rm -f "recognized.$YESTERDAY.${LOG_BUCKET}.jsonl"
        fi

        echo "Gzipping..."
        gzip -f -v -9 ./recognized."$YESTERDAY.${LOG_BUCKET}"*.jsonl

        ls -la

        echo "Uploading..."

        export GOOGLE_APPLICATION_CREDENTIALS=$HOME/logmon.json
        export CLOUDSDK_CORE_PROJECT="ncbi-logmon"
        gcloud config set account 253716305623-compute@developer.gserviceaccount.com
#        gsutil cp ./*.jsonl.gz "gs://logmon_logs_parsed/logs_${PROVIDER_LC}_public/"
        gsutil cp ./recognized."$YESTERDAY.${LOG_BUCKET}"*.jsonl.gz "gs://logmon_logs_parsed_us/logs_${PROVIDER_LC}_public/"
        #gsutil ls -lh "gs://logmon_logs_parsed/logs_${PROVIDER_LC}_public/"

        #gcloud config set account 1008590670571-compute@developer.gserviceaccount.com
        #export CLOUDSDK_CORE_PROJECT="ncbi-sandbox-blast"
        #gsutil cp ./*.jsonl.gz "gs://strides_analytics_logs_parsed/logs_gs_public/"
        #gsutil ls "gs://strides_analytics_logs_parsed/logs_gs_public/"
        cd ..
        #rm -rf "$PARSE_DEST"
    fi
done
