#!/bin/bash

# shellcheck source=/home/vartanianmh/strides/strides_env.sh
. "$HOME/strides/strides_env.sh"

# NOTES:
# * aws s3 ls is much faster than other methods on buckets with
#   huge numbers of objects
# * We copy to ramdisk to avoid seeks, since we have thousands of small files

rm -f "$LOGDIR/s3_prod/$YESTERDAY.combine"

# VDB-3892 #for x in $(seq 7); do
LOG_BUCKET="sra-pub-run-1-logs"
DEST="$RAMDISK/S3-$LOG_BUCKET/$YESTERDAY"
mkdir -p "$DEST"

# NOTE: Much faster to use aws ls than aws cp with include/excludes
echo "Copying $LOG_BUCKET into $DEST ..."
time aws s3 ls "s3://$LOG_BUCKET/$YESTERDAY_DASH" | cut -c 32- | \
    xargs -I % -P 24 aws s3 cp "s3://$LOG_BUCKET/%" "$DEST/%" --quiet
date
echo "Copied  $LOG_BUCKET  to  $DEST"

cd "$DEST" || exit
numfiles=$(find ./ -type f | wc -l)
echo "Combining $numfiles files ..."
declare -a combined
for file in ./"$YESTERDAY_DASH"*; do
    combined+=("$file")
    if [[ "${#combined[@]}" -gt 20 ]]; then
        cat "${combined[@]}" >> "$LOGDIR/s3_prod/$YESTERDAY.combine"
        unset combined
        declare -a combined
    fi
done
date
echo "Combined"

cd "$LOGDIR" || exit
echo "Processed  $LOG_BUCKET ..."
rm -rf "$DEST"
echo "Removed $DEST"

mkdir -p "$LOGDIR/s3_prod/objects"
"$HOME/strides/s3_lister.py" "sra-pub-run-1" | \
    gzip -9 -c > \
    "$LOGDIR/s3_prod/objects/$DATE.objects-1.gz"

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
cp -n -v -r "$LOGDIR/s3_prod/"* "$PANFS/s3_prod"

echo "Done"
date
