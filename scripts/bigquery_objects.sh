#!/bin/bash

# shellcheck source=strides_env.sh
. ./strides_env.sh

export CLOUDSDK_CORE_PROJECT="ncbi-logmon"
gcloud config set account 253716305623-compute@developer.gserviceaccount.com

bq rm -f -t strides_analytics.objects_uniq
bq query \
    --nouse_legacy_sql \
    --destination_table strides_analytics.objects_uniq \
    "select distinct * except (now, md5) from strides_analytics.objects"

bq rm -f -t strides_analytics.object_delta
bq query \
    --destination_table strides_analytics.object_delta \
    --nouse_legacy_sql \
    "WITH changed_objects as (
        SELECT   key, storageclass, source, bucket, count(*) as num_changes
        FROM strides_analytics.objects_uniq
        group by key, storageclass, source, bucket
        having count(*) > 1 )
        SELECT * EXCEPT (lastmodified),
            format_timestamp('%Y-%m-%dT%H:%M:%S', lastmodified) as lastmodified
  FROM ncbi-logmon.strides_analytics.objects_uniq
  INNER JOIN changed_objects USING (key, storageclass, source, bucket)"

bq extract \
    --destination_format NEWLINE_DELIMITED_JSON \
    --compression GZIP \
    'strides_analytics.object_delta' \
    "gs://logmon_export/object_delta.$DATE.*.json.gz"

echo "Extract complete"


mkdir -p "$PANFS/object_delta"
cd "$PANFS/object_delta" || exit
rm -f "$PANFS"/object_delta/object_delta."$DATE".* || true
gsutil  cp -r "gs://logmon_export/object_delta.$DATE.*" .
