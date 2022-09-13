#!/bin/bash

# shellcheck source=strides_env.sh
. ./strides_env.sh

type -a aws

buckets=$(sqlcmd "select bucket_name from buckets where cloud_provider='GS' and scope='public' and format not like '%log%' order by service_account desc")

echo "buckets is '$buckets'"

mkdir -p "$PANFS/gs_prod/objects/"

for LOG_BUCKET in $buckets; do
    profile=$(sqlcmd "select service_account from buckets where cloud_provider='GS' and bucket_name='$LOG_BUCKET'")

    echo "Getting objects for $LOG_BUCKET, profile $profile"

    "./gs_lister.py" "$LOG_BUCKET" "$profile" | \
        gzip -9 -c > \
        "$PANFS/gs_prod/objects/$DATE.gs.objects-$LOG_BUCKET.gz"

done

exit 0
