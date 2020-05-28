#!/bin/bash

# shellcheck source=strides_env.sh
. ./strides_env.sh

export GOOGLE_APPLICATION_CREDENTIALS=/home/vartanianmh/sandbox-blast-847af7ab431a.json
gcloud config set account 1008590670571-compute@developer.gserviceaccount.com
export CLOUDSDK_CORE_PROJECT="ncbi-sandbox-blast"

BUCKETS="logs_s3_public logs_s3_ca"
for bucket in $BUCKETS; do
    echo "Parsing $bucket"
    DEST="$RAMDISK/$USER/$YESTERDAY/${PREFIX}_${bucket}"
    rm -rf "$DEST"
    mkdir -p "$DEST"
    cd "$DEST" || exit
    gsutil cp "gs://${PREFIX}_${bucket}/$YESTERDAY.tar.gz" .
    for tgz in *.tar.gz; do
        tar -xaf "$tgz"
#        rm -f "$tgz"
    done
    cd "$YESTERDAY" || exit

    totalwc=0
    rm -f "$YESTERDAY.json"
    for file in *; do
        touch "$YESTERDAY.json"
        wc=$(wc -l "$file" | cut -f1 -d' ')
        totalwc=$(( totalwc + wc))
        # TODO: wc -l
        "$HOME/devel/ncbi-logging/parser/bin/log2jsn" < "$file" >> "$YESTERDAY.json" 2> "$file.err"
        rm -f "$file"
    done

    grep "{\"unrecognized\":\"" "$YESTERDAY.json" > "unrecognized.$YESTERDAY.jsonl"
    unrecwc=$(wc -l "unrecognized.$YESTERDAY.jsonl" | cut -f1 -d' ')
    grep -v "{\"unrecognized\":\"" "$YESTERDAY.json" > "recognized.$YESTERDAY.jsonl"
    recwc=$(wc -l "recognized.$YESTERDAY.jsonl" | cut -f1 -d' ')

    printf "Recognized lines:   %8d\n" "$recwc"
    printf "Unrecognized lines: %8d\n" "$unrecwc"
    printf "Total lines:        %8d\n" "$totalwc"

    rm -f "$YESTERDAY.json"

    gzip -9 ./*.jsonl

    gsutil cp ./*.jsonl.gz "gs://${PREFIX}_logs_parsed/$YESTERDAY/$bucket/"

    date
done

echo "Done"
date
