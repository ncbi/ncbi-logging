#!/bin/bash

# shellcheck source=strides_env.sh
. ./strides_env.sh

export CLOUDSDK_CORE_PROJECT="ncbi-logmon"
gcloud config set account 253716305623-compute@developer.gserviceaccount.com

# {"now": "2021-01-14 09:36:30", "bucket": "sra-pub-metadata-us-east-1", "source": "S3", "key": "sra/metadata/20210113_222240_00067_4zkhy_1b470fee-a945-471d-b0c6-f5caaec94edf", "lastmodified": "2021-01-13 22:24:26", "etag": "215a951cfa6835164c405aa47e35bc1b-11", "size": 84332183, "storageclass": "STANDARD", "md5": "215a951cfa6835164c405aa47e35bc1b-11"}

echo " #### objects "
cat << EOF > objects.json
    { "schema": { "fields": [
    { "name" : "md5", "type": "STRING" },
    { "name" : "etag", "type": "STRING" },
    { "name" : "lastmodified", "type": "STRING" },
    { "name" : "size", "type": "INTEGER" },
    { "name" : "now", "type": "STRING" },
    { "name" : "key", "type": "STRING" },
    { "name" : "storageclass", "type": "STRING" },
    { "name" : "source", "type": "STRING" },
    { "name" : "bucket", "type": "STRING" }
    ]
  },
  "sourceFormat": "NEWLINE_DELIMITED_JSON",
  "sourceUris": [
  "gs://logmon_objects/gs/2*" ]
}
EOF

jq -S -c -e . < objects.json > /dev/null
jq -S -c .schema.fields < objects.json > objects_schema_only.json

bq rm -f strides_analytics.objects_load

bq load \
    --source_format=NEWLINE_DELIMITED_JSON \
    strides_analytics.objects_load \
        "gs://logmon_objects/gs/2*" \
    objects_schema_only.json

bq load \
    --source_format=NEWLINE_DELIMITED_JSON \
    strides_analytics.objects_load \
        "gs://logmon_objects/s3/2*" \
    objects_schema_only.json

bq -q query \
--use_legacy_sql=false \
"select count(*) as summary_count from strides_analytics.objects_load"






# Reduce from 5B ($3.50) -> 50M for cost reduction
bq rm -f -t strides_analytics.objects_uniq || true
bq query \
    --nouse_legacy_sql \
    --destination_table strides_analytics.objects_uniq \
    "select distinct * except (now, md5) from strides_analytics.objects"

# TODO: Add first_appearance_accession_provider that ignores accession.version
# [DES]R[RZ][0-9]{4,17}
bq rm -f -t strides_analytics.object_delta || true
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

gsutil rm -f "gs://logmon_export/object_delta.$DATE.*" || true
bq extract \
    --destination_format NEWLINE_DELIMITED_JSON \
    --compression GZIP \
    'strides_analytics.object_delta' \
    "gs://logmon_export/object_delta.$DATE.*.json.gz"

mkdir -p "$PANFS/export/object_delta"
cd "$PANFS/export/object_delta" || exit
rm -f object_delta."$DATE".* || true
gsutil  cp -r "gs://logmon_export/object_delta.$DATE.*" .



bq rm -f -t strides_analytics.object_first_appearance || true
bq query \
    --destination_table strides_analytics.object_first_appearance \
    --nouse_legacy_sql \
    "WITH first_appear as (
      SELECT key, source,
      min(lastmodified) as first_appearance_provider
      FROM strides_analytics.objects_uniq
      GROUP BY key, source)
     SELECT distinct key, source, bucket,
            size, etag as checksum,
            lastmodified , first_appearance_provider
     FROM strides_analytics.objects_uniq
     INNER JOIN first_appear USING (key, source)"

gsutil rm -f "gs://logmon_export/object_first_appearance.$DATE.*" || true
bq extract \
    --destination_format NEWLINE_DELIMITED_JSON \
    --compression GZIP \
    'strides_analytics.object_first_appearance' \
    "gs://logmon_export/object_first_appearance.$DATE.*.json.gz"

mkdir -p "$PANFS/export/object_first_appearance"
cd "$PANFS/export/object_first_appearance" || exit
rm -f object_first_appearance."$DATE".* || true
gsutil  cp -r "gs://logmon_export/object_first_appearance.$DATE.*" .


bq rm -f -t strides_analytics.object_disappear || true
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
