#!/bin/bash

# Seed:
# rm -f days
# for d in $(seq 1 470); do
# D=$(date -d "-$d days" "+%Y_%m_%d")
# echo $D >> days
# done
# cat days | xargs -P 10 -I % ./parse.sh GS %

USAGE="Usage: $0 {S3,GS,OP} YYYY_MM_DD"

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

buckets=$(sqlcmd "select distinct log_bucket from buckets where cloud_provider='$PROVIDER' and scope='public' order by log_bucket desc")
if [ "$PROVIDER" = "OP" ]; then
    buckets="OP"
fi

echo "buckets is '$buckets'"
for LOG_BUCKET in $buckets; do
    echo "  Parsing $LOG_BUCKET..."

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

    if [ "$PROVIDER" = "OP" ]; then
        wildcard='*access*'
    else
        wildcard='*'
    fi

    echo "  Counting $TGZ ..."
    totalwc=$(tar -xaOf "$TGZ" | wc -l | cut -f1 -d' ')
    echo "  Parsing $TGZ (pattern=$wildcard), $totalwc lines ..."
    touch "$YESTERDAY_DASH.${LOG_BUCKET}.json"

    tar -xaOf "$TGZ" "$wildcard" | \
        time "$HOME/devel/ncbi-logging/parser/bin/log2jsn-rel" "$PARSER" > \
        "$YESTERDAY_DASH.${LOG_BUCKET}.json" \
        2> "$TGZ.err"
    head "$TGZ.err"
    rm -f "$TGZ"

    echo

    echo "  Record format is:"
    head -1 "$YESTERDAY_DASH.${LOG_BUCKET}.json" | jq -SM .

    set +e
    grep "\"accepted\":false," "$YESTERDAY_DASH.${LOG_BUCKET}.json" > \
        "unrecognized.$YESTERDAY_DASH.${LOG_BUCKET}.jsonl"
    grep "\"accepted\":true," "$YESTERDAY_DASH.${LOG_BUCKET}.json" > \
        "recognized.$YESTERDAY_DASH.${LOG_BUCKET}.jsonl"
    rm -f "$YESTERDAY_DASH.${LOG_BUCKET}.json"
    set -e

    # "accepted": true,
    unrecwc=$(wc -l "unrecognized.$YESTERDAY_DASH.${LOG_BUCKET}.jsonl" | cut -f1 -d' ')
    recwc=$(wc -l "recognized.$YESTERDAY_DASH.${LOG_BUCKET}.jsonl" | cut -f1 -d' ')

    printf "Recognized lines:   %8d\n" "$recwc"
    printf "Unrecognized lines: %8d\n" "$unrecwc"

#    echo "Verifying JSON..."
#    jq -e -c . < "recognized.$YESTERDAY.${LOG_BUCKET}.jsonl" > /dev/null
#    jq -e -c . < "unrecognized.$YESTERDAY.${LOG_BUCKET}.jsonl" > /dev/null

#    if [ "$unrecwc" -eq "0" ]; then
        #find ./ -name "*.jsonl" -size 0c -exec rm -f {} \;  # Don't bother with empty

#        if [ "$recwc" -gt 1000000 ]; then
#            echo "jsonl too large, splitting"
            echo "  splitting"
            split -a 3 -d -e -l 10000000 --additional-suffix=.jsonl \
                - "recognized.$YESTERDAY_DASH.${LOG_BUCKET}." \
                < "recognized.$YESTERDAY_DASH.${LOG_BUCKET}.jsonl"

            rm -f "recognized.$YESTERDAY_DASH.${LOG_BUCKET}.jsonl"
#        fi

        if [ ! -s "unrecognized.$YESTERDAY_DASH.${LOG_BUCKET}.jsonl" ]; then
            rm -f "unrecognized.$YESTERDAY_DASH.${LOG_BUCKET}.jsonl"
        fi

        echo "  Gzipping..."
        gzip -f -v -9 ./*ecognized."$YESTERDAY_DASH.${LOG_BUCKET}"*.jsonl &
        wait

        echo "  Uploading..."

        export GOOGLE_APPLICATION_CREDENTIALS=$HOME/logmon.json
        export CLOUDSDK_CORE_PROJECT="ncbi-logmon"
        gcloud config set account 253716305623-compute@developer.gserviceaccount.com
        gsutil cp ./*ecognized."$YESTERDAY_DASH.${LOG_BUCKET}"*.jsonl.gz "gs://logmon_logs_parsed_us/logs_${PROVIDER_LC}_public/"
        gsutil cp ./"$TGZ.err" "gs://logmon_logs_parsed_us/logs_${PROVIDER_LC}_public/"
        cd ..
        rm -rf "$PARSE_DEST"
#    fi
echo "  Done $LOG_BUCKET for $YESTERDAY_DASH..."
echo
done
