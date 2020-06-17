#!/bin/bash

# shellcheck source=strides_env.sh
. ./strides_env.sh

type -a aws

buckets=$(sqlcmd "select bucket_name from buckets where cloud_provider='GS' and scope = 'public' and format not like '%log%' order by service_account desc")

echo "buckets is '$buckets'"

mkdir -p "$PANFS/s3_prod/objects/"

export GOOGLE_APPLICATION_CREDENTIALS=/home/vartanianmh/nih-sra-datastore-c9b0ec6d9244.json
export CLOUDSDK_CORE_PROJECT="nih-sra-datastore"
gcloud auth activate-service-account --key-file=$GOOGLE_APPLICATION_CREDENTIALS

for LOG_BUCKET in $buckets; do
    profile=$(sqlcmd "select service_account from buckets where cloud_provider='GS' and bucket_name='$LOG_BUCKET'")

    echo "Getting objects for $LOG_BUCKET, profile $profile"

    "./gs_lister.py" "$LOG_BUCKET" | \
        gzip -9 -c > \
        "$PANFS/gs_prod/objects/$DATE.gs.objects-$LOG_BUCKET.gz" &

done

exit 0
