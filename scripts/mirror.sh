#!/bin/bash

USAGE="Usage: $0 {S3,GS,OP} YYYY_MM_DD"
if [ "$#" -ne 2 ]; then
    echo "$USAGE"
    exit 1
fi

PROVIDER=$1
DATE_UNDER=$2

# shellcheck source=strides_env.sh
. ./strides_env.sh


if [[ "$PROVIDER" = "S3" || "$PROVIDER" = "GS" || "$PROVIDER" = "OP" ]]; then
    echo
else
    echo "Invalid provider $PROVIDER"
    echo "$USAGE"
    exit 1
fi

if [[ ${#DATE_UNDER} -ne 10 ]]; then
    echo "Invalid date: $DATE_UNDER"
    echo "$USAGE"
    exit 2
fi

DATE=${DATE_UNDER//_}
DATE_DASH=${DATE_UNDER//_/-}

echo "DATE=$DATE DATE_UNDER=$DATE_UNDER DATE_DASH=$DATE_DASH"

buckets=$(sqlcmd "select distinct log_bucket from buckets where cloud_provider='$PROVIDER' order by log_bucket desc")

echo "buckets is '$buckets'"
for LOG_BUCKET in $buckets; do
    echo "Processing $LOG_BUCKET"
    MIRROR="$PANFS/$PROVIDER/$LOG_BUCKET/mirror"
    TGZ="$PANFS/$PROVIDER/$LOG_BUCKET/$DATE.tar.gz"

    mkdir -p "$MIRROR"
    cd "$MIRROR" || exit

    PROFILE=$(sqlcmd "select service_account from buckets where cloud_provider='$PROVIDER' and bucket_name='$LOG_BUCKET'")
    echo "Profile is $PROFILE, $PROVIDER rsyncing to $MIRROR..."

    if [ "$PROVIDER" = "GS" ]; then
        export GOOGLE_APPLICATION_CREDENTIALS=/home/vartanianmh/nih-sra-datastore-c9b0ec6d9244.json
        export CLOUDSDK_CORE_PROJECT="nih-sra-datastore"
        gcloud auth activate-service-account --key-file=$GOOGLE_APPLICATION_CREDENTIALS

        gsutil -m rsync "gs://$LOG_BUCKET/" .
        WILDCARD="*_usage_${DATE_UNDER}_*v0"
    fi

    if [ "$PROVIDER" = "S3" ]; then
        gsutil -m rsync "s3://$LOG_BUCKET/" .
        WILDCARD="${DATE_DASH}-*"
    fi

    echo "rsynced, tarring to $TGZ ..."

    find . -print0 -name "$WILDCARD" | tar -caf "$TGZ" --null --files-from -
    ls -l "$TGZ"

    DEST_BUCKET="gs://strides_analytics_logs_${PROVIDER}_public/"

    export GOOGLE_APPLICATION_CREDENTIALS=$HOME/sandbox-blast-847af7ab431a.json
    gcloud config set account 1008590670571-compute@developer.gserviceaccount.com
    export CLOUDSDK_CORE_PROJECT="ncbi-sandbox-blast"

    echo "Copying $TGZ to $DEST_BUCKET"
    gsutil cp "$TGZ" "$DEST_BUCKET"
    echo "Done with $LOG_BUCKET"
done
echo "Done"
