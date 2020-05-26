#!/bin/bash

# shellcheck source=/home/vartanianmh/strides/strides_env.sh
. "$HOME/strides/strides_env.sh"

buckets=$(sqlcmd "select distinct log_bucket from buckets where cloud_provider='S3'")
# TODO: Use sqlcmd
# NOTES:
# * aws s3 ls is much faster than other methods on buckets with
#   huge numbers of objects
# * We copy to ramdisk to avoid seeks, since we have thousands of small files

LOG_BUCKET="sra-pub-run-1-logs"
DEST="$RAMDISK/S3-$LOG_BUCKET/$YESTERDAY"
mkdir -p "$DEST"

type -a aws

# NOTE: Much faster to use aws ls than aws cp with include/excludes
echo "Copying $LOG_BUCKET into $DEST ..."
time aws s3 ls "s3://$LOG_BUCKET/$YESTERDAY_DASH" | cut -c 32- | \
    xargs -I % -P 24 aws s3 cp "s3://$LOG_BUCKET/%" "$DEST/%" --quiet
date
echo "Copied  $LOG_BUCKET  to  $DEST"

tar -caf $PANFS/s3_prod/$YESTERDAY.tar.gz $DEST

cd "$LOGDIR" || exit
echo "Processed  $LOG_BUCKET ..."
rm -rf "$DEST"
echo "Removed $DEST"

export GOOGLE_APPLICATION_CREDENTIALS=/home/vartanianmh/sandbox-blast-847af7ab431a.json
gcloud config set account 1008590670571-compute@developer.gserviceaccount.com
export CLOUDSDK_CORE_PROJECT="ncbi-sandbox-blast"

gsutil -m rsync -r "$LOGDIR/s3_prod/" gs://strides_analyticsTODOBUCKET/s3_prod/

echo "Done"
date
