#!/bin/bash

# shellcheck source=strides_env.sh
. ./strides_env.sh

export CLOUDSDK_CORE_PROJECT="ncbi-logmon"
gcloud config set account 253716305623-compute@developer.gserviceaccount.com

# Reduce from 5B ($3.50) -> 50M for cost reduction
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

gsutil rm -f "gs://logmon_export/object_delta.$DATE.*"
bq extract \
    --destination_format NEWLINE_DELIMITED_JSON \
    --compression GZIP \
    'strides_analytics.object_delta' \
    "gs://logmon_export/object_delta.$DATE.*.json.gz"

mkdir -p "$PANFS/export/object_delta"
cd "$PANFS/export/object_delta" || exit
rm -f object_delta."$DATE".* || true
gsutil  cp -r "gs://logmon_export/object_delta.$DATE.*" .



bq rm -f -t strides_analytics.object_first_appearance
bq query \
    --destination_table strides_analytics.object_first_appearance \
    --nouse_legacy_sql \
    "WITH first_appear as (
      SELECT key, source,
      min(lastmodified) as first_appearance_provider
      FROM strides_analytics.objects_uniq
      GROUP BY key, source)
     SELECT distinct key, source, bucket,
            size as size, etag as checksum,
            lastmodified , first_appearance_provider
     FROM strides_analytics.objects_uniq
     INNER JOIN first_appear USING (key, source)"

gsutil rm -f "gs://logmon_export/object_first_appearance.$DATE.*"
bq extract \
    --destination_format NEWLINE_DELIMITED_JSON \
    --compression GZIP \
    'strides_analytics.object_first_appearance' \
    "gs://logmon_export/object_first_appearance.$DATE.*.json.gz"

mkdir -p "$PANFS/export/object_first_appearance"
cd "$PANFS/export/object_first_appearance" || exit
rm -f object_first_appearance."$DATE".* || true
gsutil  cp -r "gs://logmon_export/object_first_appearance.$DATE.*" .


bq rm -f -t strides_analytics.object_disappear
bq query \
    --destination_table strides_analytics.object_disappear \
    --nouse_legacy_sql \
    "WITH last_appear as (
    SELECT key, source, bucket, max(now) as last_seen
    FROM strides_analytics.objects
    GROUP BY key, source, bucket )
    SELECT *
    FROM last_appear
    WHERE last_seen < timestamp_sub(current_timestamp(), INTERVAL 5 DAY)"
