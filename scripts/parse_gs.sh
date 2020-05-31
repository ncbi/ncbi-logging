#!/bin/bash

# shellcheck source=strides_env.sh
. ./strides_env.sh

export GOOGLE_APPLICATION_CREDENTIALS=/home/vartanianmh/sandbox-blast-847af7ab431a.json
gcloud config set account 1008590670571-compute@developer.gserviceaccount.com
export CLOUDSDK_CORE_PROJECT="ncbi-sandbox-blast"

#YESTERDAY="20200530"

BUCKETS="logs_gs_public logs_gs_ca"
for bucket in $BUCKETS; do
    DEST="$RAMDISK/$USER/$YESTERDAY/${PREFIX}_${bucket}"
    rm -rf "$DEST"
    mkdir -p "$DEST"
    cd "$DEST" || exit
    echo "Copying ${PREFIX}_${bucket} to $DEST ..."
    gsutil -q -m cp "gs://${PREFIX}_${bucket}/$YESTERDAY/*_usage_*gz" .

    echo "Copied ${PREFIX}_{$bucket}"
    totalwc=0
    for file in ./*.gz; do
        wc=$(zcat "$file" | wc -l | cut -f1 -d' ')
        echo -n "Parsing $file, $wc lines ... "
        totalwc=$(( totalwc + wc))
        zcat "$file" | time "$HOME/devel/ncbi-logging/parser/bin/log2jsn" gcp > "$file.json" 2> "$file.err"
        newwc=$(wc -l "$file".json | cut -f1 -d' ')
        echo -n "Parsed, $newwc lines emitted"
        if [ "$wc" -ne "$newwc" ]; then
            echo "***** Linecount discrepancy *****"
        fi
        echo

        rm -f "$file"
    done

    echo "Combining..."
    rm -f "$YESTERDAY.json"
    for file in ./*.json; do
        touch "$YESTERDAY.json"
        cat "$file" >> "$YESTERDAY.json"
    done
    echo "Combined"

    set +e
    grep "{\"unrecognized\":\"" "$YESTERDAY.json" > "unrecognized.$YESTERDAY.jsonl"
    grep -v "{\"unrecognized\":\"" "$YESTERDAY.json" > "recognized.$YESTERDAY.jsonl"
    set -e

    unrecwc=$(wc -l "unrecognized.$YESTERDAY.jsonl" | cut -f1 -d' ')
    recwc=$(wc -l "recognized.$YESTERDAY.jsonl" | cut -f1 -d' ')

    printf "Recognized lines:   %8d\n" "$recwc"
    printf "Unrecognized lines: %8d\n" "$unrecwc"
    printf "Total lines:        %8d\n" "$totalwc"

    rm -f "$YESTERDAY.json"

    jq -e -c . < "recognized.$YESTERDAY.jsonl" > /dev/null
    jq -e -c . < "unrecognized.$YESTERDAY.jsonl" > /dev/null

    # Don't bother with empty
    find ./ -name "*.jsonl" -size 0c -exec rm -f {} \;
    gzip -v -9 ./*.jsonl

    gsutil cp ./*.jsonl.gz "gs://${PREFIX}_logs_parsed/$bucket/"
    gsutil ls -l "gs://${PREFIX}_logs_parsed/$bucket/"
    date
    rm -rf "$DEST"
done

echo "Done"
date
