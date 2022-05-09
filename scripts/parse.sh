#!/bin/bash

# Seed:
# rm -f days
# for d in $(seq 1 470); do
# D=$(date -d "-$d days" "+%Y_%m_%d")
# echo $D >> days
# done
# cat days | xargs -P 10 -I % ./parse.sh GS %

USAGE="Usage: $0 {S3,GS,OP,Splunk} YYYY_MM_DD"

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
    Splunk)
        export PARSER="cl"
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

buckets=$(sqlcmd "select distinct log_bucket from buckets where cloud_provider='$PROVIDER' and scope='$STRIDES_SCOPE' order by log_bucket desc")
readarray -t buckets <<< "$buckets"
echo "buckets has ${#buckets[@]}, is ' " "${buckets[*]}" "'"

if [ "$PROVIDER" = "OP" ]; then
    buckets=("OP-srafiles11" "OP-srafiles12" "OP-srafiles13" "OP-srafiles21" "OP-srafiles22" "OP-srafiles23" "OP-srafiles31" "OP-srafiles32" "OP-srafiles33" "OP-srafiles34" "OP-srafiles35" "OP-srafiles36" "OP-ftp" "OP-ftp1" "OP-ftp11" "OP-ftp12" "OP-ftp13" "OP-ftp2" "OP-ftp21" "OP-ftp22" "OP-ftp31" "OP-ftp32" "OP-ftp33" "OP")
fi

df -HT .
for LOG_BUCKET in "${buckets[@]}"; do
    echo "  LOG_BUCKET=$LOG_BUCKET"
    PARSE_DEST="$TMP/parsed/$PROVIDER/$LOG_BUCKET/$YESTERDAY"
    mkdir -p "$PARSE_DEST"
    cd "$PARSE_DEST" || exit

    SRC_BUCKET="gs://logmon_logs/${PROVIDER_LC}_${STRIDES_SCOPE}/"
    TGZ="$YESTERDAY_DASH.$LOG_BUCKET.tar.gz"
    echo "  Copying $TGZ to $PARSE_DEST"

    export CLOUDSDK_CORE_PROJECT="ncbi-logmon"
    gcloud config set account 253716305623-compute@developer.gserviceaccount.com
    gsutil -o 'GSUtil:sliced_object_download_threshold=0' cp "${SRC_BUCKET}${TGZ}" . || true
    if [ ! -s "$TGZ" ]; then
        continue
    fi
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


    echo "  Parsing $LOG_BUCKET..."
    if [ "$PARSER" = "cl" ]; then
        VERSION=$("$HOME"/devel/ncbi-logging/parser/bin/cl2jsn-rel --version)

        tar -xaOf "$TGZ" "$wildcard" | \
            tr -s ' ' | \
            time "$HOME/devel/ncbi-logging/parser/bin/cl2jsn-rel" -f "$PARSER" > \
            "$YESTERDAY_DASH.${LOG_BUCKET}.out" \
            2> "$TGZ.err"

            echo "  Record format is:"
            head -1 "cl.good.jsonl" | jq -SM .

            set +e
            cat cl.unrecog.jsonl cl.review.jsonl cl.bad.jsonl > \
                "unrecognized.$YESTERDAY_DASH.${LOG_BUCKET}.jsonl"
            mv cl.good.jsonl \
                "recognized.$YESTERDAY_DASH.${LOG_BUCKET}.jsonl"
            set -e

    else
        VERSION=$("$HOME"/devel/ncbi-logging/parser/bin/log2jsn-rel --version)
        ls -l "$HOME"/devel/ncbi-logging/parser/bin/log2jsn-rel

        # sed to work around """linux64 bug (LOGMON-208)
        # The seds require about 15% the CPU of log2jsn-rel
        tar -xaOf "$TGZ" "$wildcard" | \
            sed 's/"""linux64""/"linux64/g'  | \
            sed 's/"""linux64"/"linux64/g'  | \
            sed  's/""linux64"/"linux64/g'  | \
            sed  's/""mac64"/"mac64/g'  | \
            sed  's/""windows64"/"windows64/g'  | \
            time "$HOME/devel/ncbi-logging/parser/bin/log2jsn-rel" "$PARSER" > \
            "$YESTERDAY_DASH.${LOG_BUCKET}.json" \
            2> "$TGZ.err"

        echo "  Record format is:"
        head -1 "$YESTERDAY_DASH.${LOG_BUCKET}.json" | jq -SM .

        set +e
        grep "\"accepted\":false," "$YESTERDAY_DASH.${LOG_BUCKET}.json" > \
            "unrecognized.$YESTERDAY_DASH.${LOG_BUCKET}.jsonl"
        grep "\"accepted\":true," "$YESTERDAY_DASH.${LOG_BUCKET}.json" > \
            "recognized.$YESTERDAY_DASH.${LOG_BUCKET}.jsonl"
        rm -f "$YESTERDAY_DASH.${LOG_BUCKET}.json"
        set -e
    fi
    head "$TGZ.err"
    rm -f "$TGZ"

    unrecwc=$(wc -l "unrecognized.$YESTERDAY_DASH.${LOG_BUCKET}.jsonl" | cut -f1 -d' ')
    recwc=$(wc -l "recognized.$YESTERDAY_DASH.${LOG_BUCKET}.jsonl" | cut -f1 -d' ')

    echo

    printf "Recognized lines:   %8d\n" "$recwc"
    printf "Unrecognized lines: %8d\n" "$unrecwc"

    echo "  splitting"
    split -a 3 -d -e -l 20000000 --additional-suffix=.jsonl \
        - "recognized.$YESTERDAY_DASH.${LOG_BUCKET}." \
        < "recognized.$YESTERDAY_DASH.${LOG_BUCKET}.jsonl"

    rm -f "recognized.$YESTERDAY_DASH.${LOG_BUCKET}.jsonl"

    if [ ! -s "unrecognized.$YESTERDAY_DASH.${LOG_BUCKET}.jsonl" ]; then
        rm -f "unrecognized.$YESTERDAY_DASH.${LOG_BUCKET}.jsonl"
    fi

    echo "  Gzipping..."
    gzip -f -v -9 ./*ecognized."$YESTERDAY_DASH.${LOG_BUCKET}"*.jsonl &
    wait

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

    echo "  Uploading to gs://logmon_logs_parsed_us/logs_${PROVIDER_LC}_${STRIDES_SCOPE}/ ..."

    export GOOGLE_APPLICATION_CREDENTIALS=$HOME/logmon.json
    export CLOUDSDK_CORE_PROJECT="ncbi-logmon"
    gcloud config set account 253716305623-compute@developer.gserviceaccount.com
    gsutil cp ./*ecognized."$YESTERDAY_DASH.${LOG_BUCKET}"*.jsonl.gz "gs://logmon_logs_parsed_us/logs_${PROVIDER_LC}_${STRIDES_SCOPE}/"
    gsutil cp ./"$TGZ.err" "gs://logmon_logs_parsed_us/logs_${PROVIDER_LC}_${STRIDES_SCOPE}/"
    gsutil cp ./"summary.$TGZ.jsonl" "gs://logmon_logs_parsed_us/logs_${PROVIDER_LC}_${STRIDES_SCOPE}/"
    cd ..
    rm -rf "$PARSE_DEST"

    echo "  Done $LOG_BUCKET for $YESTERDAY_DASH..."
done
