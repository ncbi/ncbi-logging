#!/bin/bash

# shellcheck source=strides_env.sh
. ./strides_env.sh

export CLOUDSDK_CORE_PROJECT="ncbi-logmon"
gcloud config set account 253716305623-compute@developer.gserviceaccount.com

#gsutil ls -lR gs://logmon_logs_parsed_us/


####### gs_parsed
cat << EOF > gs_schema.json
    { "schema": { "fields": [
    { "name" : "accepted", "type": "BOOLEAN" },
    { "name" : "accession", "type": "STRING" },
    { "name" : "agent", "type": "STRING" },
    { "name" : "bucket", "type": "STRING" },
    { "name" : "extension", "type": "STRING" },
    { "name" : "host", "type": "STRING" },
    { "name" : "ip", "type": "STRING" },
    { "name" : "ip_region", "type": "STRING" },
    { "name" : "ip_type", "type": "INTEGER" },
    { "name" : "method", "type": "STRING" },
    { "name" : "operation", "type": "STRING" },
    { "name" : "path", "type": "STRING" },
    { "name" : "referer", "type": "STRING" },
    { "name" : "request_bytes", "type": "INTEGER" },
    { "name" : "request_id", "type": "STRING" },
    { "name" : "result_bytes", "type": "INTEGER" },
    { "name" : "source", "type": "STRING" },
    { "name" : "status", "type": "INTEGER" },
    { "name" : "time", "type": "INTEGER" },
    { "name" : "time_taken", "type": "INTEGER" },
    { "name" : "uri", "type": "STRING" },
    { "name" : "vers", "type": "STRING" }
    ]
  },
  "sourceFormat": "NEWLINE_DELIMITED_JSON",
  "sourceUris": [ "gs://logmon_logs_parsed_us/logs_gs_public/recognized.*" ]
}
EOF

    jq -c -e . < gs_schema.json > /dev/null
    jq -c .schema.fields < gs_schema.json > gs_schema_only.json

    #bq mk --external_table_definition=gs_schema_only.son strides_analytics.gs_parsed

    gsutil ls -lR gs://logmon_logs_parsed_us/logs_gs_public/recognized.*

    bq rm -f strides_analytics.gs_parsed
    bq load \
        --source_format=NEWLINE_DELIMITED_JSON \
        strides_analytics.gs_parsed \
        "gs://logmon_logs_parsed_us/logs_gs_public/recognized.*" \
        gs_schema_only.json
    bq show --schema strides_analytics.gs_parsed



####### s3_parsed
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
    "sourceUris": [ "gs://logmon_logs_parsed_us/logs_s3_public/recogn*" ]
    }
EOF
    jq -c -e . < s3_schema.json > /dev/null
    jq -c .schema.fields < s3_schema.json > s3_schema_only.json

    gsutil ls -lR gs://logmon_logs_parsed_us/logs_s3_public/recognized.*

    bq rm -f strides_analytics.s3_parsed
    bq load \
        --max_bad_records 500 \
        --source_format=NEWLINE_DELIMITED_JSON \
        strides_analytics.s3_parsed \
        "gs://logmon_logs_parsed_us/logs_s3_public/recognized.*" \
        s3_schema_only.json
    bq show --schema strides_analytics.s3_parsed


####### op_parsed


####### gs_fixed
    QUERY=$(cat <<-ENDOFQUERY
    SELECT
    ip as remote_ip,
        parse_datetime('%s', cast ( cast (time/1000000 as int64) as string) ) as start_ts,
    datetime_add(
        parse_datetime('%s', cast ( cast (time/1000000 as int64) as string) ),
        interval time_taken microsecond) as end_ts,
    agent as user_agent,
    cast (status as string) as http_status,
    host as host,
    method as http_operation,
    path as request_uri,
    referer as referer,
    regexp_extract(path,r'[DES]R[RZ][0-9]{5,10}') as accession,
    result_bytes as bytes_sent,
    substr(regexp_extract(path,r'[0-9]\.[0-9]{1,2}'),3) as version,
    case
        WHEN regexp_contains(bucket, r'-run-') THEN bucket || ' (ETL + BQS)'
        WHEN regexp_contains(bucket, r'-zq-') THEN bucket || ' (ETL - BQS)'
        WHEN regexp_contains(bucket, r'-src-') THEN bucket || ' (Original)'
        WHEN regexp_contains(bucket, r'-ca-') THEN bucket || ' (Controlled Access)'
    ELSE bucket || ' (Unknown)'
    END as bucket,
    source as source,
    current_datetime() as fixed_time
    FROM \\\`ncbi-logmon.strides_analytics.gs_parsed\\\`
ENDOFQUERY
)

    # TODO Hack, cause I can't understand bash backtick quoting
    QUERY="${QUERY//\\/}"
    bq rm --project_id ncbi-logmon -f strides_analytics.gs_fixed
    # shellcheck disable=SC2016
    bq query \
    --project_id ncbi-logmon \
    --destination_table strides_analytics.gs_fixed \
    --use_legacy_sql=false \
    --batch=true \
    "$QUERY"
    bq show --schema strides_analytics.gs_fixed


####### s3_fixed
    # LOGMON-1: Remove multiple -heads from agent for S3
    QUERY=$(cat <<-ENDOFQUERY
    SELECT
    ip as remote_ip,
    parse_datetime('%d.%m.%Y:%H:%M:%S 0', time) as start_ts,
    datetime_add(parse_datetime('%d.%m.%Y:%H:%M:%S 0', time),
        interval cast (
                case when total_time='' THEN '0' ELSE total_time END
        as int64) millisecond) as end_ts,
    regexp_extract(path,r'[DES]R[RZ][0-9]{5,10}') as accession,
    substr(regexp_extract(path,r'[0-9]\.[0-9]{1,2}'),3) as version,
    case
        WHEN regexp_contains(agent, r'-head') THEN 'HEAD'
        ELSE method
        END as http_operation,
    cast(res_code as string) as http_status,
    host_header as host,
    cast (case WHEN res_len='' THEN '0' ELSE res_len END as int64) as bytes_sent,
    path as request_uri,
    referer as referer,
    replace(agent, '-head', '') as user_agent,
    source as source,
    case
        WHEN regexp_contains(bucket, r'-run-') THEN bucket || ' (ETL + BQS)'
        WHEN regexp_contains(bucket, r'-zq-') THEN bucket || ' (ETL - BQS)'
        WHEN regexp_contains(bucket, r'-src-') THEN bucket || ' (Original)'
        WHEN regexp_contains(bucket, r'-ca-') THEN bucket || ' (Controlled Access)'
    ELSE bucket || ' (Unknown)'
    END as bucket,
    current_datetime() as fixed_time
    FROM \\\`ncbi-logmon.strides_analytics.s3_parsed\\\`
    WHERE accepted=true
ENDOFQUERY
    )

    QUERY="${QUERY//\\/}"
    bq rm --project_id ncbi-logmon -f strides_analytics.s3_fixed
    # shellcheck disable=SC2016
    bq query \
    --project_id ncbi-logmon \
    --destination_table strides_analytics.s3_fixed \
    --use_legacy_sql=false \
    --batch=true \
    "$QUERY"
    bq show --schema strides_analytics.s3_fixed


####### op_parsed
    cat << EOF > op_test_schema.json
    [
    { "name": "accepted", "type": "BOOLEAN" },
    { "name": "agent", "type": "STRING" },
    { "name": "forwarded", "type": "STRING" },
    { "name": "ip", "type": "STRING" },
    { "name": "port", "type": "INTEGER" },
    { "name": "referer", "type": "STRING" },
    { "name": "req_len", "type": "INTEGER" },
    { "name": "req_time", "type": "STRING" },
    { "name": "request", "type": "RECORD", "mode": "REPEATED",
    "fields": [
            { "name": "method", "type": "STRING" },
            { "name": "params", "type": "STRING" },
            { "name": "path", "type": "STRING" },
            { "name": "server", "type": "STRING" },
            { "name": "vers", "type": "STRING" }
        ]
    },
    { "name": "res_code", "type": "INTEGER" },
    { "name": "res_len", "type": "INTEGER" },
    { "name": "source", "type": "STRING" },
    { "name": "time", "type": "STRING" },
    { "name": "user", "type": "STRING" }
    ]
EOF

    jq -e -c . < op_test_schema.json > /dev/null
    jq -c .schema.fields < op_schema.json > op_schema_only.json
    bq rm  -f strides_analytics.op_test
    bq mk \
        --table strides_analytics.op_test \
        ./op_test_schema.json

exit 0
####### detail_export
    QUERY=$(cat <<-ENDOFQUERY
SELECT
    accession as accession,
    bucket as bucket,
    bytes_sent as bytes_sent,
    current_datetime() as export_time,
    end_ts,
    fixed_time as fixed_time,
    host as host,
    http_status as http_status,
    remote_ip as remote_ip,
    http_operation as http_operation,
    request_uri as request_uri,
    referer as referer,
    source as source,
    start_ts,
    user_agent as user_agent,
    version as version
    FROM \\\`strides_analytics.gs_fixed\\\`
UNION ALL SELECT
    accession as accession,
    bucket as bucket,
    bytes_sent as bytes_sent,
    current_datetime() as export_time,
    end_ts,
    fixed_time as fixed_time,
    host as host,
    http_status as http_status,
    remote_ip as remote_ip,
    http_operation as http_operation,
    request_uri as request_uri,
    referer as referer,
    source as source,
    start_ts,
    user_agent as user_agent,
    version as version
    FROM \\\`strides_analytics.s3_fixed\\\`

ENDOFQUERY
    )

    QUERY="${QUERY//\\/}"

    bq rm --project_id ncbi-logmon -f strides_analytics.detail_export
    # shellcheck disable=SC2016
    bq query \
    --project_id ncbi-logmon \
    --destination_table strides_analytics.detail_export \
    --use_legacy_sql=false \
    --batch=true \
    "$QUERY"

    bq show --schema strides_analytics.detail_export

###### summary_export
    QUERY=$(cat <<-ENDOFQUERY
    SELECT
    accession,
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
    WHERE start_ts > '2020-01-01'
    GROUP BY accession, user_agent, remote_ip, host, bucket, source
    HAVING bytes_sent > 0
ENDOFQUERY
    )

    QUERY="${QUERY//\\/}"

    bq rm --project_id ncbi-logmon -f strides_analytics.summary_export
    # shellcheck disable=SC2016
    bq query \
    --destination_table strides_analytics.summary_export \
    --use_legacy_sql=false \
    --batch=true \
    "$QUERY"

    bq show --schema strides_analytics.summary_export


###### export to GS
    gsutil rm -f "gs://strides_analytics/detail/detail.$DATE.*.json.gz"
    bq extract \
    --destination_format NEWLINE_DELIMITED_JSON \
    --compression GZIP \
    'strides_analytics.detail_export' \
    "gs://strides_analytics/detail/detail.$DATE.*.json.gz"

    gsutil rm -f "gs://logmon_export/summary/summary.$DATE.*.json.gz"
    bq extract \
    --destination_format NEWLINE_DELIMITED_JSON \
    --compression GZIP \
    'strides_analytics.summary_export' \
    "gs://logmon_export/summary/summary.$DATE.*.json.gz"


###### copy to filesystem
    mkdir -p "$PANFS/detail"
    cd "$PANFS/detail" || exit
    rm -f "$PANFS"/detail/detail."$DATE".* || true
    gsutil  cp -r "gs://strides_analytics/detail/detail.$DATE.*" "$PANFS/detail/"

    mkdir -p "$PANFS/summary"
    cd "$PANFS/summary" || exit
    rm -f "$PANFS"/summary/summary."$DATE".* || true
    gsutil  cp -r "gs://strides_analytics/summary/summary.$DATE.*" "$PANFS/summary/"

date
