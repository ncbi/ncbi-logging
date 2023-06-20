#!/bin/bash

# shellcheck source=strides_env.sh
. ./strides_env.sh

type -a aws

buckets=$(sqlcmd "select bucket_name from buckets where cloud_provider='S3' and scope='public' and format not like '%log%' order by service_account desc")

echo "buckets is '$buckets'"

mkdir -p "$PANFS/s3_prod/objects/"

for BUCKET in $buckets; do
    profile=$(sqlcmd "select service_account from buckets where cloud_provider='S3' and bucket_name='$BUCKET'")

    if [[ $BUCKET == *"logs"* ]]; then
        echo "Skipping log bucket $BUCKET"
    else
        echo "Getting objects for $BUCKET, profile $profile"

        "./s3_lister.py" "$BUCKET" "$profile" |
            gzip -9 -c > \
                "$PANFS/s3_prod/objects/$DATE.s3.objects-$BUCKET.gz" &
    fi
done

wait

exit 0
