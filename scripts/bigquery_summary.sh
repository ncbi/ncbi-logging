#!/usr/bin/bash

# shellcheck source=strides_env.sh
. ./strides_env.sh

export CLOUDSDK_CORE_PROJECT="ncbi-logmon"
gcloud config set account 253716305623-compute@developer.gserviceaccount.com

cat << EOF > summary_schema.json
    { "schema": { "fields": [
    { "name" : "log_bucket", "type": "STRING" },
    { "name" : "provider", "type": "STRING" },
    { "name" : "log_date", "type": "STRING" },
    { "name" : "parse_date", "type": "STRING" },
    { "name" : "parser_version", "type": "STRING" },
    { "name" : "total_lines", "type": "INTEGER" },
    { "name" : "recognized_lines", "type": "INTEGER" },
    { "name" : "unrecognized_lines", "type": "INTEGER" }
        ]
    },
  "sourceFormat": "NEWLINE_DELIMITED_JSON",
  "sourceUris": [
  "gs://logmon_logs_parsed_us/logs_s3_public/summary.*"
  ]
}
EOF

jq -S -c -e . < summary_schema.json > /dev/null
jq -S -c .schema.fields < summary_schema.json > summary_schema_only.json

bq rm -f strides_analytics.summary
#bq mk --external_table_definition=summary_schema_only.json strides_analytics.summary

bq load \
    --source_format=NEWLINE_DELIMITED_JSON \
    strides_analytics.summary \
    "gs://logmon_logs_parsed_us/logs_gs_public/summary.*,gs://logmon_logs_parsed_us/logs_op_public/summary.*,gs://logmon_logs_parsed_us/logs_s3_public/summary.*" \
    summary_schema_only.json
bq show --schema strides_analytics.summary

bq -q query \
    --use_legacy_sql=false \
    "select log_bucket, provider, string_agg(distinct parser_version) as parser_versions, sum(total_lines) as total_lines, sum(recognized_lines) as recognized_lines, sum(unrecognized_lines) as unrecognized_lines, count(*) as num_logs from strides_analytics.summary group by log_bucket, provider order by log_bucket, provider"

date
