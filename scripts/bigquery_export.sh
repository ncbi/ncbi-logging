#!/bin/bash

# shellcheck source=strides_env.sh
. ./strides_env.sh

export CLOUDSDK_CORE_PROJECT="ncbi-logmon"
gcloud config set account 253716305623-compute@developer.gserviceaccount.com

gsutil ls -lR gs://logmon_logs_parsed_us/

cat << EOF > gs_schema.json
{ "schema": { "fields": [
    { "name" : "accepted", "type": "BOOLEAN" },
    { "name" : "accession", "type": "STRING" },
    { "name" : "agent", "type": "STRING" },
    { "name" : "extension", "type": "STRING" },
    { "name" : "forwarded", "type": "STRING" },
    { "name" : "ip", "type": "STRING" },
    { "name" : "method", "type": "STRING" },
    { "name" : "path", "type": "STRING" },
    { "name" : "port", "type": "INTEGER" },
    { "name" : "referer", "type": "STRING" },
    { "name" : "req_len", "type": "INTEGER" },
    { "name" : "req_time", "type": "STRING" },
    { "name" : "res_code", "type": "INTEGER" },
    { "name" : "res_len", "type": "INTEGER" },
    { "name" : "server", "type": "STRING" },
    { "name" : "source", "type": "STRING" },
    { "name" : "time", "type": "STRING" },
    { "name" : "user", "type": "STRING" },
    { "name" : "vers", "type": "STRING" }
    ]
  },
  "sourceFormat": "NEWLINE_DELIMITED_JSON",
  "sourceUris": [ "gs://logmon_logs_parsed_us/logs_gs_public/recognized.*" ]
}
EOF

jq -c -e . < gs_schema.json

bq rm -f strides_analytics.gs_parsed
bq mk --external_table_definition=gs_schema.json strides_analytics.gs_parsed



cat << EOF > s3_schema.json
{ "schema": { "fields": [
    { "name" : "accepted", "type": "BOOLEAN" },
    { "name" : "accession", "type": "STRING" },
    { "name" : "agent", "type": "STRING" },
    { "name" : "auth_type", "type": "STRING" },
    { "name" : "bucket", "type": "STRING" },
    { "name" : "cipher_suite", "type": "STRING" },
    { "name" : "error", "type": "STRING" },
    { "name" : "extension", "type": "STRING" },
    { "name" : "host_header", "type": "STRING" },
    { "name" : "host_id", "type": "STRING" },
    { "name" : "ip", "type": "STRING" },
    { "name" : "key", "type": "STRING" },
    { "name" : "method", "type": "STRING" },
    { "name" : "obj_size", "type": "STRING" },
    { "name" : "operation", "type": "STRING" },
    { "name" : "owner", "type": "STRING" },
    { "name" : "path", "type": "STRING" },
    { "name" : "referer", "type": "STRING" },
    { "name" : "request_id", "type": "STRING" },
    { "name" : "requester", "type": "STRING" },
    { "name" : "res_code", "type": "STRING" },
    { "name" : "res_len", "type": "STRING" },
    { "name" : "source", "type": "STRING" },
    { "name" : "time", "type": "STRING" },
    { "name" : "tls_version", "type": "STRING" },
    { "name" : "total_time", "type": "STRING" },
    { "name" : "turnaround_time", "type": "STRING" },
    { "name" : "vers", "type": "STRING" },
    { "name" : "version_id", "type": "STRING" }
    ]
  },
  "sourceFormat": "NEWLINE_DELIMITED_JSON",
  "sourceUris": [ "gs://logmon_logs_parsed_us/logs_s3_public/20*" ]
}
EOF

jq -c -e . < s3_schema.json


bq rm -f strides_analytics.s3_parsed
bq mk --external_table_definition=s3_schema.json strides_analytics.s3_parsed


if [[ "$DATE" == "false" ]]; then
    cat << EOF > op_test_schema.json
    [
    { "name": "accepted", "type": "BOOLEAN", "mode": "NULLABLE" },
    { "name": "agent", "type": "STRING", "mode": "NULLABLE" },
    { "name": "forwarded", "type": "STRING", "mode": "NULLABLE" },
    { "name": "ip", "type": "STRING", "mode": "NULLABLE" },
    { "name": "port", "type": "INTEGER", "mode": "NULLABLE" },
    { "name": "referer", "type": "STRING", "mode": "NULLABLE" },
    { "name": "req_len", "type": "INTEGER", "mode": "NULLABLE" },
    { "name": "req_time", "type": "STRING", "mode": "NULLABLE" },
    { "name": "request", "type": "RECORD", "mode": "REPEATED",
    "fields": [
            { "name": "method", "type": "STRING", "mode": "NULLABLE" },
            { "name": "params", "type": "STRING", "mode": "NULLABLE" },
            { "name": "path", "type": "STRING", "mode": "NULLABLE" },
            { "name": "server", "type": "STRING", "mode": "NULLABLE" },
            { "name": "vers", "type": "STRING", "mode": "NULLABLE" }
        ]
    },
    { "name": "res_code", "type": "INTEGER", "mode": "NULLABLE" },
    { "name": "res_len", "type": "INTEGER", "mode": "NULLABLE" },
    { "name": "source", "type": "STRING", "mode": "NULLABLE" },
    { "name": "time", "type": "STRING", "mode": "NULLABLE" },
    { "name": "user", "type": "STRING", "mode": "NULLABLE" }
    ]
EOF
        bq rm  -f strides_analytics.s3_test
        bq mk \
            --table \
            strides_analytics.s3_test \
            accepted:BOOLEAN,agent:STRING,auth_type:STRING,bucket:STRING,cipher_suite:STRING,error:STRING,host_header:STRING,host_id:STRING,ip:STRING,key:STRING,obj_size:INTEGER,operation:STRING,owner:STRING,referer:STRING,request:STRING,request_id:STRING,requester:STRING,res_code:INTEGER,res_len:INTEGER,source:STRING,time:STRING,tls_version:STRING,total_time:INTEGER,turnaround_time:INTEGER,version_id:STRING


        jq -e -c . < op_test_schema.json
        bq rm  -f strides_analytics.op_test
        bq mk \
            --table strides_analytics.op_test \
            ./op_test_schema.json


    bq load \
        --autodetect \
        --source_format=NEWLINE_DELIMITED_JSON \
        strides_analytics.s3_test \
        "gs://strides_analytics_logs_parsed/logs_s3_public/recognized.${YESTERDAY}.jsonl.gz"

    bq load \
        --source_format=NEWLINE_DELIMITED_JSON \
        strides_analytics.op_test \
        "gs://strides_analytics_logs_parsed/sra_prod/recognized.${YESTERDAY}.jsonl.gz"
fi



# TODO: Use BigQuery parameters instead of shell substitution?
QUERY=$(cat <<-ENDOFQUERY
SELECT
  ip as remote_ip,
  parse_datetime('%d.%m.%Y:%H:%M:%S 0', time) as start_ts,
  datetime_add(parse_datetime('%d.%m.%Y:%H:%M:%S 0', time), interval cast (total_time as int64) millisecond) as end_ts,
  case
      WHEN regexp_contains(agent, r'-head') THEN 'HEAD'
      ELSE method
      END as http_operation,
      path as request_uri,
  cast(res_code as string) as http_status,
  ifnull(res_len, '0') as bytes_sent,
  referer as referer,
  replace(agent, '-head', '') as user_agent,
  host_header as host,
  bucket as bucket,
  source as source,
  current_datetime() as export_time
  FROM \\\`strides_analytics.s3_parsed\\\`
ENDOFQUERY
)

# TODO Hack, cause I can't understand bash backtick quoting
QUERY="${QUERY//\\/}"

echo "Query is $QUERY"

bq show --schema strides_analytics.s3_parsed

bq rm -f strides_analytics.detail_export


# shellcheck disable=SC2016
bq query \
    --destination_table strides_analytics.detail_export \
    --use_legacy_sql=false \
    --batch=true \
    "$QUERY"

bq show --schema strides_analytics.detail_export

QUERY2=$(cat <<-ENDOFQUERY
SELECT
    request_uri,
    user_agent,
    remote_ip,
    host,
    bucket,
    source,
    count(*) as num_requests,
    min(start_ts) as start_ts,
    max(end_ts) as end_ts,
    string_agg(distinct http_operation order by http_operation) as http_operations,
    string_agg(distinct http_status order by http_status) as http_statuses,
    string_agg(distinct referer) as referers,
    sum(bytes_sent) as bytes_sent,
    current_datetime() as export_time,
    "TBD: city" as city_name,
    "TBD: country" as country,
    "TBD: region" as region,
    "TBD: example.com" as domain
  FROM \\\`strides_analytics.detail_export\\\`
  GROUP BY request_uri, user_agent, remote_ip, host, bucket, source
  HAVING bytes_sent > 0
ENDOFQUERY
)

# TODO Hack, cause I can't understand bash backtick quoting
QUERY2="${QUERY2//\\/}"

echo "Query is $QUERY2"

# shellcheck disable=SC2016
bq query \
    --destination_table strides_analytics.summary_export \
    --use_legacy_sql=false \
    --batch=true \
    "$QUERY2"

bq show --schema strides_analytics.summary_export

echo "Queries complete"

gsutil rm -f "gs://strides_analytics/detail/detail.$DATE.*.json.gz"
bq extract \
    --destination_format NEWLINE_DELIMITED_JSON \
    --compression GZIP \
    'strides_analytics.detail_export' \
    "gs://strides_analytics/detail/detail.$DATE.*.json.gz"

gsutil rm -f "gs://strides_analytics/summary/summary.$DATE.*.json.gz"
bq extract \
    --destination_format NEWLINE_DELIMITED_JSON \
    --compression GZIP \
    'strides_analytics.summary_export' \
    "gs://strides_analytics/summary/summary.$DATE.*.json.gz"

echo "Extract complete"

bq rm -f strides_analytics.detail_export
bq rm -f strides_analytics.summary_export

mkdir -p "$PANFS/detail"
cd "$PANFS/detail" || exit
rm -f "$PANFS"/detail/detail."$DATE".* || true
gsutil  cp -r "gs://strides_analytics/detail/detail.$DATE.*" "$PANFS/detail/"

mkdir -p "$PANFS/summary"
cd "$PANFS/summary" || exit
rm -f "$PANFS"/summary/summary."$DATE".* || true
gsutil  cp -r "gs://strides_analytics/summary/summary.$DATE.*" "$PANFS/summary/"

date
