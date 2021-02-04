#!/bin/bash

# shellcheck source=strides_env.sh
. ./strides_env.sh

export CLOUDSDK_CORE_PROJECT="ncbi-logmon"
gcloud config set account 253716305623-compute@developer.gserviceaccount.com

# {"now": "2021-01-14 09:36:30", "bucket": "sra-pub-metadata-us-east-1", "source": "S3", "key": "sra/metadata/20210113_222240_00067_4zkhy_1b470fee-a945-471d-b0c6-f5caaec94edf", "lastmodified": "2021-01-13 22:24:26", "etag": "215a951cfa6835164c405aa47e35bc1b-11", "size": 84332183, "storageclass": "STANDARD", "md5": "215a951cfa6835164c405aa47e35bc1b-11"}

echo " #### loading objects "
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
    --max_bad_records 500 \
    strides_analytics.objects_load \
        "gs://logmon_objects/gs/2*" \
    objects_schema_only.json

bq load \
    --source_format=NEWLINE_DELIMITED_JSON \
    --max_bad_records 500 \
    strides_analytics.objects_load \
        "gs://logmon_objects/s3/2*" \
    objects_schema_only.json

bq -q query \
--use_legacy_sql=false \
"select source, bucket, count(*) as object_load_count from strides_analytics.objects_load group by source, bucket"


echo " #### parsing objects "
bq rm -f -t strides_analytics.objects || true
bq query \
    --destination_table strides_analytics.objects \
    --nouse_legacy_sql \
    --max_rows=5 \
    "SELECT * except (now, lastmodified),
    safe.parse_timestamp('%Y-%m-%d %H:%M:%S', now) as now,
    safe.parse_timestamp('%Y-%m-%d %H:%M:%S', lastmodified) as lastmodified
    FROM strides_analytics.objects_load"
bq -q query \
--use_legacy_sql=false \
"select count(*) as objects_count from strides_analytics.objects"



echo " #### objects_uniq "
# Reduce from 5B ($3.50) -> 50M for cost reduction
bq rm -f -t strides_analytics.objects_uniq || true
bq query \
    --destination_table strides_analytics.objects_uniq \
    --nouse_legacy_sql \
    --max_rows=5 \
    "select distinct * except (now, md5) from strides_analytics.objects"
bq -q query \
--use_legacy_sql=false \
"select source, bucket, count(*) as object_uniq_count, min(lastmodified), max(lastmodified) from strides_analytics.objects_uniq group by source, bucket order by source, bucket"


echo " #### object_delta"
# TODO: Add first_appearance_accession_provider that ignores accession.version
# [DES]R[RZ][0-9]{4,17}
bq rm -f -t strides_analytics.object_delta || true
bq query \
    --destination_table strides_analytics.object_delta \
    --nouse_legacy_sql \
    --max_rows=5 \
    "WITH changed_objects as (
        SELECT   key, storageclass, source, bucket, count(*) as num_changes
        FROM strides_analytics.objects_uniq
        group by key, storageclass, source, bucket
        having count(*) > 1 )
        SELECT * EXCEPT (lastmodified),
            format_timestamp('%Y-%m-%dT%H:%M:%S', lastmodified) as lastmodified
  FROM ncbi-logmon.strides_analytics.objects_uniq
  INNER JOIN changed_objects USING (key, storageclass, source, bucket)"
bq -q query \
--use_legacy_sql=false \
"select count(*) as object_delta_count from strides_analytics.object_delta"

bq -q query \
--use_legacy_sql=false \
"select source, bucket, count(*) as object_delta_cnt, min(lastmodified), max(lastmodified) from strides_analytics.object_delta group by source, bucket order by source, bucket"


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



echo " #### object_first_appearance"
bq rm -f -t strides_analytics.object_first_appearance || true
bq query \
    --destination_table strides_analytics.object_first_appearance \
    --nouse_legacy_sql \
    --max_rows=5 \
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
bq -q query \
--use_legacy_sql=false \
"select source, bucket, count(*) as object_first_count from strides_analytics.object_first_appearance group by source, bucket order by source, bucket"


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


echo " #### object_disappear"
bq rm -f -t strides_analytics.object_disappear || true
bq query \
    --destination_table strides_analytics.object_disappear \
    --nouse_legacy_sql \
    --max_rows=5 \
    "WITH last_appear as (
    SELECT key, source, bucket, max(now) as last_seen
    FROM strides_analytics.objects
    GROUP BY key, source, bucket )
    SELECT *
    FROM last_appear
    WHERE last_seen < timestamp_sub(current_timestamp(), INTERVAL 5 DAY)"
bq -q query \
--use_legacy_sql=false \
"select source, bucket, count(*) as object_disappear_count from strides_analytics.object_disappear group by source, bucket order by source, bucket"
