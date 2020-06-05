#!/bin/bash

# shellcheck source=strides_env.sh
. ./strides_env.sh

export GOOGLE_APPLICATION_CREDENTIALS=$HOME/sandbox-blast-847af7ab431a.json
gcloud config set account 1008590670571-compute@developer.gserviceaccount.com
export CLOUDSDK_CORE_PROJECT="ncbi-sandbox-blast"

#YESTERDAY="20200602"

BUCKETS="logs_s3_public logs_s3_ca"
for bucket in $BUCKETS; do
    echo "Parsing $bucket"
    DEST="$RAMDISK/$USER/$YESTERDAY/${PREFIX}_${bucket}"
    rm -rf "$DEST"
    mkdir -p "$DEST"
    cd "$DEST" || exit
    gsutil cp "gs://${PREFIX}_${bucket}/$YESTERDAY.tar.gz" .
    for tgz in *.tar.gz; do
        echo "Extracting $tgz"
        set +e
        tar -xaOf "$tgz" | gzip -1 -c  > "$tgz.combine.gz"
        set -e
        rm -f "$tgz"
    done
    #cd "$YESTERDAY" || exit
    echo "Extracted"

    totalwc=0
    rm -f "$YESTERDAY.json"
    for file in *.combine.gz; do
        echo "Counting $file"
        wc=$(zcat "$file" | wc -l | cut -f1 -d' ')
        echo "Parsing $file, $wc lines"
        totalwc=$(( totalwc + wc))
        touch "$YESTERDAY.json"
        zcat "$file" | time "$HOME/devel/ncbi-logging/parser/bin/log2jsn-rel" >> "$YESTERDAY.json" 2> "$file.err"
        newwc=$(wc -l "$YESTERDAY".json | cut -f1 -d' ')
        echo "Parsed $file, $newwc lines emitted"
        if [ "$wc" -ne "$newwc" ]; then
            echo "***** Linecount discrepancy *****"
        fi

        rm -f "$file"
    done

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

    echo "Verifying JSON..."
    # Remove leading spaces and slashes and pluses
    #sed -i "s/^[ \t]*//" "unrecognized.$YESTERDAY.jsonl"
    #sed -i "s/^[+\/]*//" "unrecognized.$YESTERDAY.jsonl"
    #sed -i "s/^[ \t]*//" "recognized.$YESTERDAY.jsonl"
    #sed -i "s/^[+\/]*//" "recognized.$YESTERDAY.jsonl"

    jq -e -c . < "recognized.$YESTERDAY.jsonl" > /dev/null
    jq -e -c . < "unrecognized.$YESTERDAY.jsonl" > /dev/null

    echo "Gzipping..."
    # Don't bother with empty
    find ./ -name "*.jsonl" -size 0c -exec rm -f {} \;
    gzip -9 ./*.jsonl

    echo "Uploading..."
    gsutil -m cp ./*.jsonl.gz "gs://${PREFIX}_logs_parsed/$bucket/"
    gsutil ls -l "gs://${PREFIX}_logs_parsed/$bucket/"
    date
    rm -rf "$DEST"
done

echo "Done"
date
