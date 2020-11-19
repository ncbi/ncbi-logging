#!/bin/bash

# shellcheck source=strides_env.sh
. ./strides_env.sh

export GOOGLE_APPLICATION_CREDENTIALS=$HOME/sandbox-blast-847af7ab431a.json
gcloud config set account 1008590670571-compute@developer.gserviceaccount.com
export CLOUDSDK_CORE_PROJECT="ncbi-sandbox-blast"

#YESTERDAY="20200601"
MISSING="20200601 20200602 20200603 20200604 20200605 20200606 20200607 20200609 20200609 20200610"

#BUCKETS="logs_gs_public logs_gs_ca"
BUCKETS="logs_gs_public"

for YESTERDAY in $MISSING; do
    for bucket in $BUCKETS; do
        DEST="$RAMDISK/$USER/$YESTERDAY/gs_new/${PREFIX}_${bucket}"
        rm -rf "$DEST"
        mkdir -p "$DEST"
        cd "$DEST" || exit
        pwd
        echo "Copying ${PREFIX}_${bucket} to $DEST ..."
        gsutil -q -m cp "gs://${PREFIX}_${bucket}/$YESTERDAY/*_usage_*gz" .

        echo "Copied ${PREFIX}_${bucket}"

        echo "Combining..."
        unset combined
        declare -a combined
        for file in ./*.gz; do
            combined+=("$file")
            if [[ "${#combined[@]}" -gt 20 ]]; then
                zcat "${combined[@]}" >> "$YESTERDAY.combine"
                rm -f "${combined[@]}"
                unset combined
                declare -a combined
            fi
        done
        zcat "${combined[@]}" >> "$YESTERDAY.combine"
        rm -f "${combined[@]}"
        unset combined

        echo -n "Parsing ... "
        time "$HOME/devel/ncbi-logging/parser/bin/log2jsn-rel" gcp < "$YESTERDAY".combine > "$YESTERDAY.${bucket}.json" 2> "$YESTERDAY.${bucket}.err"
        echo
        head -v ./*.err

        set +e
        grep "{\"unrecognized\":\"" "$YESTERDAY.${bucket}.json" > "unrecognized.$YESTERDAY.${bucket}.jsonl"
        grep -v "{\"unrecognized\":\"" "$YESTERDAY.${bucket}.json" > "recognized.$YESTERDAY.${bucket}.jsonl"
        set -e

        unrecwc=$(wc -l "unrecognized.$YESTERDAY.${bucket}.jsonl" | cut -f1 -d' ')
        recwc=$(wc -l "recognized.$YESTERDAY.${bucket}.jsonl" | cut -f1 -d' ')

        printf "Recognized lines:   %8d\n" "$recwc"
        printf "Unrecognized lines: %8d\n" "$unrecwc"

        echo "Verifying JSON..."
        jq -e -c . < "recognized.$YESTERDAY.${bucket}.jsonl" > /dev/null
        jq -e -c . < "unrecognized.$YESTERDAY.${bucket}.jsonl" > /dev/null

        if [ "$unrecwc" -eq "0" ]; then
            echo "Gzipping..."
            find ./ -name "*.jsonl" -size 0c -exec rm -f {} \;  # Don't bother with empty
            gzip -v -9 ./*.jsonl

            ls -la

            echo "Uploading..."
            gsutil cp ./*.jsonl.gz "gs://${PREFIX}_logs_parsed/$bucket/"
            gsutil cp ./*.jsonl.gz "gs://${PREFIX}_logs_parsed_us/$bucket/"
            cd ..
            rm -rf "$DEST"
        fi

        gsutil ls -l "gs://${PREFIX}_logs_parsed/$bucket/"
    done
done
