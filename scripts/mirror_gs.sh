#!/bin/bash

# shellcheck source=strides_env.sh
. ./strides_env.sh

buckets=$(sqlcmd "select distinct log_bucket from buckets where cloud_provider='GS' order by log_bucket desc")

echo "buckets is '$buckets'"
for LOG_BUCKET in $buckets; do
    echo "Processing $LOG_BUCKET"
    DEST="$PANFS/gs_prod2/$LOG_BUCKET/$YESTERDAY"
    mkdir -p "$DEST"
    cd "$DEST" || exit

    # TODO: Fetch scope for destination bucket
    if [[ $LOG_BUCKET =~ "-pub-" ]]; then
        export GOOGLE_APPLICATION_CREDENTIALS=$HOME/nih-sra-datastore-c9b0ec6d9244.json
        export CLOUDSDK_CORE_PROJECT="nih-sra-datastore"
        gcloud auth activate-service-account --key-file="$GOOGLE_APPLICATION_CREDENTIALS"

        DEST_BUCKET="gs://strides_analytics_logs_gs_public"
    fi

    if [[ $LOG_BUCKET =~ "-ca-" ]]; then
        DEST_BUCKET="gs://strides_analytics_logs_gs_ca"
    fi

    gsutil -q -m cp "gs://$LOG_BUCKET/*_$YESTERDAY_UNDER*_v0" .

    echo "gzipping..."
    time find ./ -name "*_v0" -exec gzip -9 -f {} \;
    echo "Processed  $LOG_BUCKET"

    export GOOGLE_APPLICATION_CREDENTIALS=$HOME/sandbox-blast-847af7ab431a.json
    gcloud config set account 1008590670571-compute@developer.gserviceaccount.com
    export CLOUDSDK_CORE_PROJECT="ncbi-sandbox-blast"

    echo "Copying from $DEST to $DEST_BUCKET/$YESTERDAY"
    gsutil -q -m cp "$DEST/*" "$DEST_BUCKET/$YESTERDAY/"
    echo
done

echo "Done"
date
