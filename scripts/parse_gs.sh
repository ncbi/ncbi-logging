#!/bin/bash

# shellcheck source=strides_env.sh
. ./strides_env.sh

export GOOGLE_APPLICATION_CREDENTIALS=$HOME/sandbox-blast-847af7ab431a.json
gcloud config set account 1008590670571-compute@developer.gserviceaccount.com
export CLOUDSDK_CORE_PROJECT="ncbi-sandbox-blast"

#YESTERDAY="20200601"

BUCKETS="logs_gs_public logs_gs_ca"
BUCKETS="logs_gs_public"
for bucket in $BUCKETS; do
    DEST="$RAMDISK/$USER/$YESTERDAY/${PREFIX}_${bucket}"
    rm -rf "$DEST"
    mkdir -p "$DEST"
    cd "$DEST" || exit
    echo "Copying ${PREFIX}_${bucket} to $DEST ..."
    gsutil -q -m cp "gs://${PREFIX}_${bucket}/$YESTERDAY/*_usage_*gz" .

    echo "Copied ${PREFIX}_${bucket}"
    totalwc=0
    for file in ./*.gz; do
        wc=$(zcat "$file" | wc -l | cut -f1 -d' ')
        echo -n "Parsing $file, $wc lines ... "
        totalwc=$(( totalwc + wc))
        zcat "$file" | time "$HOME/devel/ncbi-logging/parser/bin/log2jsn-rel" gcp > "$file.${bucket}.json" 2> "$file.err"
        newwc=$(wc -l "$file"."${bucket}".json | cut -f1 -d' ')
        echo -n "Parsed, $newwc lines emitted"
        expected=$(( newwc + 1 )) # Ignore header
        if [ "$wc" -ne "$expected" ]; then
            echo "***** Linecount discrepancy *****"
            exit 1
        fi
        echo

        rm -f "$file"
    done

    echo "Combining..."
    rm -f "$YESTERDAY.${bucket}.json"
    for file in ./*.json; do
        touch "$YESTERDAY.${bucket}.json"
        cat "$file" >> "$YESTERDAY.${bucket}.json"
    done
    echo "Combined"

    set +e
    grep "{\"unrecognized\":\"" "$YESTERDAY.${bucket}.json" > "unrecognized.$YESTERDAY.${bucket}.jsonl"
    grep -v "{\"unrecognized\":\"" "$YESTERDAY.${bucket}.json" > "recognized.$YESTERDAY.${bucket}.jsonl"
    set -e

    unrecwc=$(wc -l "unrecognized.$YESTERDAY.${bucket}.jsonl" | cut -f1 -d' ')
    recwc=$(wc -l "recognized.$YESTERDAY.${bucket}.jsonl" | cut -f1 -d' ')

    printf "Recognized lines:   %8d\n" "$recwc"
    printf "Unrecognized lines: %8d\n" "$unrecwc"
    printf "Total lines:        %8d\n" "$totalwc"

    rm -f "$YESTERDAY.${bucket}.json"

    echo "Verifying JSON..."
    jq -e -c . < "recognized.$YESTERDAY.${bucket}.jsonl" > /dev/null
    jq -e -c . < "unrecognized.$YESTERDAY.${bucket}.jsonl" > /dev/null

    # Don't bother with empty
    echo "Gzipping..."
    find ./ -name "*.jsonl" -size 0c -exec rm -f {} \;
    gzip -v -9 ./*.jsonl

export GOOGLE_APPLICATION_CREDENTIALS=$HOME/sandbox-blast-847af7ab431a.json
gcloud config set account 1008590670571-compute@developer.gserviceaccount.com
export CLOUDSDK_CORE_PROJECT="ncbi-sandbox-blast"

    echo "Uploading..."
    gsutil cp ./*.jsonl.gz "gs://${PREFIX}_logs_parsed/$bucket/"
    gsutil cp ./*.jsonl.gz "gs://${PREFIX}_logs_parsed_us/$bucket/"
    gsutil ls -l "gs://${PREFIX}_logs_parsed/$bucket/"
    #rm -rf "$DEST"
done
#done
echo "Done"
