#!/bin/bash

# shellcheck source=strides_env.sh
. ./strides_env.sh

type -a aws

buckets=$(sqlcmd "select bucket_name from buckets where cloud_provider='GS' and format not like '%log%' order by service_account desc")

echo "buckets is '$buckets'"

mkdir -p "$PANFS/s3_prod/objects/"

for LOG_BUCKET in $buckets; do
    profile=$(sqlcmd "select service_account from buckets where cloud_provider='GS' and bucket_name='$LOG_BUCKET'")

    echo "Getting objects for $LOG_BUCKET, profile $profile"

    "./s3_lister.py" "$LOG_BUCKET" | \
        gzip -9 -c > \
        "$PANFS/s3_prod/objects/$DATE.gs.objects-$LOG_BUCKET.gz" &

    ls -l "$PANFS/s3_prod/objects/$DATE.gs.objects-$LOG_BUCKET.gz"

done

exit 0
