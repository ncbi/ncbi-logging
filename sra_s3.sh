#!/bin/bash

# shellcheck source=/home/vartanianmh/strides/strides_env.sh
. "$HOME/strides/strides_env.sh"

# NOTES:
# * aws s3 ls is much faster than other methods on buckets with
#   huge numbers of objects
# * We copy to ramdisk to avoid seeks, since we have thousands of small files

rm -f "$LOGDIR/s3_prod/$YESTERDAY.combine"

for x in $(seq 7); do
    LOG_BUCKET="sra-pub-run-$x-logs"
    DEST="$RAMDISK/S3-$LOG_BUCKET/$YESTERDAY"
    mkdir -p "$DEST"

    echo "Processing  $LOG_BUCKET into $DEST"
    time aws s3 ls "s3://$LOG_BUCKET/$YESTERDAY_DASH" | cut -c 32- | \
        xargs -I % -P 24 aws s3 cp "s3://$LOG_BUCKET/%" "$DEST/%" --quiet

    cd "$DEST" || exit
    for file in ./*; do
        cat "$file" >> "$LOGDIR/s3_prod/$YESTERDAY.combine"
    done
    cd "$LOGDIR" || exit
    echo "Processed  $LOG_BUCKET"
    rm -rf "$DEST"
    echo "Removed $DEST"

    mkdir -p "$LOGDIR/s3_prod/objects"
    "$HOME/strides/s3_lister.py" "sra-pub-run-$x" | \
        gzip -9 -c > \
        "$LOGDIR/s3_prod/objects/$DATE.objects-$x.gz"
done

time "$HOME/strides/s3tojson" < \
    "$LOGDIR/s3_prod/$YESTERDAY.combine" | \
    gzip -9 -c > "$LOGDIR/s3_prod/$YESTERDAY.jsonl.gz" \
    2> "$LOGDIR/s3_prod/$YESTERDAY.err"

time gzip -9 "$LOGDIR/s3_prod/$YESTERDAY.combine"

#    ./s3_agent.py "$LOG_BUCKET" 'SRA@S3'

export GOOGLE_APPLICATION_CREDENTIALS=/home/vartanianmh/sandbox-blast-847af7ab431a.json
gcloud config set account 1008590670571-compute@developer.gserviceaccount.com
export CLOUDSDK_CORE_PROJECT="ncbi-sandbox-blast"

gsutil -m rsync -r "$LOGDIR/s3_prod/" gs://strides_analytics/s3_prod/
cp -n -v "$LOGDIR/s3_prod/"*gz $PANFS/s3_prod/

echo "Done"
date
