#!/bin/bash

# shellcheck source=strides_env.sh
. ./strides_env.sh

type -a aws

buckets=$(sqlcmd "select distinct log_bucket from buckets where cloud_provider='S3' order by log_bucket desc")

#YESTERDAY="20200530"
#YESTERDAY_DASY="2020-05-30"

echo "buckets is '$buckets'"
for LOG_BUCKET in $buckets; do
    # NOTES:
    # * aws s3 ls is much faster than other methods on buckets with
    #   huge numbers of objects
    # * We copy to ramdisk to avoid seeks, since we have thousands of small files

    DEST="$TMP/S3-$LOG_BUCKET/$YESTERDAY"
    mkdir -p "$DEST"
    cd "$DEST" || exit

# TODO: Fetch service account for AWS --profile
    # NOTE: Much faster to use aws ls than aws cp with include/excludes
    set +e
    objects=$(aws s3 ls "s3://$LOG_BUCKET/$YESTERDAY_DASH" | cut -c 32- )
    wc=$(echo "$objects" | wc -l)
    echo "Copying $wc objects from $LOG_BUCKET into $DEST"
    echo "$objects" | xargs -I % -P 24 aws s3 cp "s3://$LOG_BUCKET/%" "$DEST/%" --quiet
    set -e
    date
    echo "Copied $LOG_BUCKET to $DEST"
    TAR="$PANFS/s3_prod/$YESTERDAY.tar.gz"

    cd "$DEST/.." || exit
    tar -caf "$TAR" "$YESTERDAY"

    echo "Processed  $LOG_BUCKET ..."
    rm -rf "$DEST"
    echo "Removed $DEST"

    export GOOGLE_APPLICATION_CREDENTIALS=/home/vartanianmh/sandbox-blast-847af7ab431a.json
    gcloud config set account 1008590670571-compute@developer.gserviceaccount.com
    export CLOUDSDK_CORE_PROJECT="ncbi-sandbox-blast"

# TODO: Fetch scope for destination bucket
    if [[ $LOG_BUCKET =~ "-pub-" ]]; then
        DEST_BUCKET="gs://strides_analytics_logs_s3_public"
    fi
    if [[ $LOG_BUCKET =~ "-ca-" ]]; then
        DEST_BUCKET="gs://strides_analytics_logs_s3_ca"
    fi

    gsutil cp "$TAR" "$DEST_BUCKET"
    gsutil ls -l "$DEST_BUCKET"

    rm -rf "$DEST"
done

echo "Done"
date
