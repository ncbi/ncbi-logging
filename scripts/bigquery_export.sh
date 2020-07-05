#!/usr/bin/bash

# shellcheck source=strides_env.sh
. ./strides_env.sh

export CLOUDSDK_CORE_PROJECT="ncbi-logmon"
gcloud config set account 253716305623-compute@developer.gserviceaccount.com


gsutil du -s -h gs://logmon_logs/gs_public
gsutil du -s -h gs://logmon_logs/s3_public
gsutil du -s -h gs://logmon_logs_parsed_us/logs_gs_public/
gsutil du -s -h gs://logmon_logs_parsed_us/logs_s3_public/

# TODO: Partition/cluster large tables for incremental inserts and retrievals
# TODO: Materialized views that automatically refresh

echo " #### gs_parsed"
cat << EOF > gs_schema.json
    { "schema": { "fields": [
    { "name" : "accepted", "type": "BOOLEAN" },
    { "name" : "accession", "type": "STRING" },
    { "name" : "agent", "type": "STRING" },
    { "name" : "bucket", "type": "STRING" },
    { "name" : "extension", "type": "STRING" },
    { "name" : "filename", "type": "STRING" },
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

    gsutil ls -lR "gs://logmon_logs_parsed_us/logs_gs_public/recognized.*"

    bq rm -f strides_analytics.gs_parsed
    bq load \
        --source_format=NEWLINE_DELIMITED_JSON \
        strides_analytics.gs_parsed \
        "gs://logmon_logs_parsed_us/logs_gs_public/recognized.*" \
        gs_schema_only.json
    bq show --schema strides_analytics.gs_parsed



echo " #### s3_parsed"
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
        { "name" : "filename", "type": "STRING" },
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

    gsutil ls -lR "gs://logmon_logs_parsed_us/logs_s3_public/recognized.*"

    bq rm -f strides_analytics.s3_parsed
    bq load \
        --max_bad_records 500 \
        --source_format=NEWLINE_DELIMITED_JSON \
        strides_analytics.s3_parsed \
        "gs://logmon_logs_parsed_us/logs_s3_public/recognized.*" \
        s3_schema_only.json

    bq show --schema strides_analytics.s3_parsed


echo " #### op_parsed"
# TODO


echo " #### Parsed results"
bq -q query \
    --use_legacy_sql=false \
    "select source, accepted, min(time) as min_time, max(time) as max_time, count(*) as parsed_count from (select source, accepted, cast(time as string) as time from strides_analytics.gs_parsed union all select source, accepted, cast(time as string) as time from strides_analytics.s3_parsed) group by source, accepted order by source"

echo " #### gs_fixed"
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
        WHEN ends_with(bucket, '-cov2') and
            regexp_contains(path, r'sra-src') THEN bucket || ' (Original)'
        WHEN ends_with(bucket, '-cov2') and
            regexp_contains(path, r'run') THEN bucket || ' (ETL + BQS)'
    ELSE bucket || ' (Unknown)'
    END as bucket,
    source as source,
    current_datetime() as fixed_time
    FROM \\\`ncbi-logmon.strides_analytics.gs_parsed\\\`
    WHERE accepted=true
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
    --max_rows=10 \
    "$QUERY"

    bq show --schema strides_analytics.gs_fixed



    #parse_datetime('%d.%m.%Y:%H:%M:%S 0', time) as start_ts,
    #[17/May/2019:23:19:24 +0000]
echo " #### s3_fixed"
    # LOGMON-1: Remove multiple -heads from agent for S3
    QUERY=$(cat <<-ENDOFQUERY
    SELECT
    ip as remote_ip,
    parse_datetime('[%d/%b/%Y:%H:%M:%S +0000]', time) as start_ts,
    datetime_add(parse_datetime('[%d/%b/%Y:%H:%M:%S +0000]', time),
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
        WHEN ends_with(bucket, '-cov2') and
            regexp_contains(path, r'sra-src') THEN bucket || ' (Original)'
        WHEN ends_with(bucket, '-cov2') and
            regexp_contains(path, r'run') THEN bucket || ' (ETL + BQS)'
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
    --max_rows=10 \
    "$QUERY"

    bq show --schema strides_analytics.s3_fixed


echo " ##### op_parsed"
    cat << EOF > op_schema.json
    { "schema": { "fields": [
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
    },
    "sourceFormat": "NEWLINE_DELIMITED_JSON",
    "sourceUris": [ "gs://logmon_logs_parsed_us/logs_op_public/recogn*" ]
    }
EOF

    jq -e -c . < op_schema.json > /dev/null
    jq -c .schema.fields < op_schema.json > op_schema_only.json
    bq rm  -f strides_analytics.op_test
    #bq mk \
    #   --table strides_analytics.op_test \
    #    ./op_schema.json

echo " ##### detail_export"
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
    --max_rows=10 \
    "$QUERY"

    bq show --schema strides_analytics.detail_export

echo " ###  summary_grouped"
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
    current_datetime() as export_time
    FROM \\\`strides_analytics.detail_export\\\`
    WHERE start_ts > '2000-01-01'
    GROUP BY accession, user_agent, remote_ip, host, bucket, source, datetime_trunc(start_ts, day),
    case
        WHEN http_operation in ('GET', 'HEAD') THEN 0
        WHEN http_operation='POST' THEN 1
        WHEN http_operation='PUT' THEN 2
        WHEN http_operation='DELETE' THEN 3
        WHEN http_operation='PATCH' THEN 4
        WHEN http_operation='OPTIONS' THEN 5
        WHEN http_operation='TRACE' THEN 6
        WHEN http_operation='CONNECT' THEN 7
    ELSE 99
    END
    HAVING bytes_sent > 0
ENDOFQUERY
    )

    QUERY="${QUERY//\\/}"

    bq rm --project_id ncbi-logmon -f strides_analytics.summary_grouped
    # shellcheck disable=SC2016
    bq query \
    --destination_table strides_analytics.summary_grouped \
    --use_legacy_sql=false \
    --batch=true \
    --max_rows=10 \
    "$QUERY"

    bq show --schema strides_analytics.summary_grouped

echo " ###  iplookup_new"
    QUERY=$(cat <<-ENDOFQUERY
    SELECT DISTINCT remote_ip, net.ipv4_to_int64(net.safe_ip_from_string(remote_ip)) as ipint,
    FROM \\\`ncbi-logmon.strides_analytics.summary_grouped\\\`
    WHERE remote_ip like '%.%'
ENDOFQUERY
)
    QUERY="${QUERY//\\/}"

    bq rm --project_id ncbi-logmon -f strides_analytics.uniq_ips
    # shellcheck disable=SC2016
    bq query \
    --destination_table strides_analytics.uniq_ips \
    --use_legacy_sql=false \
    --batch=true \
    --max_rows=10 \
    "$QUERY"

RUN="yes"
if [ "$RUN" = "yes" ]; then
# Only needs to be running when a lot of new IP addresses appear
    QUERY=$(cat <<-ENDOFQUERY
    SELECT remote_ip, ipint,
    country_code, region_name, city_name
    FROM \\\`ncbi-logmon.strides_analytics.ip2location\\\`
    JOIN \\\`ncbi-logmon.strides_analytics.uniq_ips\\\`
    ON (ipint >= ip_from and ipint <= ip_to)
ENDOFQUERY
)
    QUERY="${QUERY//\\/}"

    bq rm --project_id ncbi-logmon -f strides_analytics.iplookup_new
    # shellcheck disable=SC2016
    bq query \
    --destination_table strides_analytics.iplookup_new \
    --use_legacy_sql=false \
    --batch=true \
    --max_rows=10 \
    "$QUERY"
fi

echo " ###  summary_export"
    QUERY=$(cat <<-ENDOFQUERY
    SELECT
    accession,
    user_agent,
    grouped.remote_ip,
    host,
    bucket,
    grouped.source,
    num_requests,
    start_ts,
    end_ts,
    http_operations,
    http_statuses,
    referers,
    bytes_sent,
    current_datetime() as export_time,
    case
        WHEN rdns.domain is null or rdns.domain='' or rdns.domain='Unknown' or rdns.domain='unknown'
            THEN 'Unknown (' || country_code || ')'
        ELSE rdns.domain
    END as domain,
    region_name,
    country_code,
    city_name,
    ScientificName,
    cast (size_mb as int64) as accession_size_mb
    FROM \\\`strides_analytics.summary_grouped\\\` grouped
    LEFT JOIN \\\`strides_analytics.rdns\\\` rdns
        ON grouped.remote_ip=rdns.ip
    LEFT JOIN \\\`strides_analytics.iplookup_new\\\` iplookup_new
        ON grouped.remote_ip=iplookup_new.remote_ip
    LEFT JOIN \\\`strides_analytics.public_fix\\\`
        ON accession=run
    WHERE
        accession IS NOT NULL AND accession != ""
ENDOFQUERY
    )

    QUERY="${QUERY//\\/}"

        #--project_id ncbi-logmon \
    bq \
        cp strides_analytics.summary_export "strides_analytics.summary_export_$YESTERDAY"
    bq rm --project_id ncbi-logmon -f strides_analytics.summary_export
    # shellcheck disable=SC2016
    bq query \
    --destination_table strides_analytics.summary_export \
    --use_legacy_sql=false \
    --batch=true \
    --max_rows=10 \
    "$QUERY"
    bq show --schema strides_analytics.summary_export

    OLD=$(date -d "-5 days" "+%Y%m%d")
    bq rm --project_id ncbi-logmon -f "strides_analytics.summary_export_$OLD"



echo " ###  export to GS"
    gsutil rm -f "gs://logmon_export/detail/detail.$DATE.*.json.gz" || true
#    bq extract \
#    --destination_format NEWLINE_DELIMITED_JSON \
#    --compression GZIP \
#    'strides_analytics.detail_export' \
#    "gs://logmon_export/detail/detail.$DATE.*.json.gz"

    gsutil rm -f "gs://logmon_export/summary/summary.$DATE.*.json.gz" || true
    bq extract \
    --destination_format NEWLINE_DELIMITED_JSON \
    --compression GZIP \
    'strides_analytics.summary_export' \
    "gs://logmon_export/summary/summary.$DATE.*.json.gz"

    gsutil rm -f "gs://logmon_export/uniq_ips/uniq_ips.$DATE.*.json.gz" || true
    bq extract \
    --destination_format NEWLINE_DELIMITED_JSON \
    --compression GZIP \
    'strides_analytics.uniq_ips' \
    "gs://logmon_export/uniq_ips/uniq_ips.$DATE.json.gz"



echo " ###  copy to filesystem"
#    mkdir -p "$PANFS/detail"
#    cd "$PANFS/detail" || exit
#    rm -f "$PANFS"/detail/detail."$DATE".* || true
#    gsutil  cp -r "gs://logmon_export/detail/detail.$DATE.*" "$PANFS/detail/"

    mkdir -p "$PANFS/summary"
    cd "$PANFS/summary" || exit
    rm -f "$PANFS"/summary/summary."$DATE".* || true
    gsutil  cp -r "gs://logmon_export/summary/summary.$DATE.*" "$PANFS/summary/"

    mkdir -p "$PANFS/uniq_ips"
    cd "$PANFS/uniq_ips" || exit
    rm -f "$PANFS"/uniq_ips/uniq_ips."$DATE".* || true
    gsutil  cp -r "gs://logmon_export/uniq_ips/uniq_ips.$DATE.*" "$PANFS/uniq_ips/"



date
