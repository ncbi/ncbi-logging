#!/usr/bin/bash

# shellcheck source=strides_env.sh
. ./strides_env.sh

export CLOUDSDK_CORE_PROJECT="ncbi-logmon"
gcloud config set account 253716305623-compute@developer.gserviceaccount.com


echo " #### cloudian_summary"
cat << EOF > cloudian_summary_schema.json
    { "schema": { "fields": [
    { "name" : "log_bucket", "type": "STRING" },
    { "name" : "log_date", "type": "STRING" },
    { "name" : "parse_date", "type": "STRING" },
    { "name" : "parser_version", "type": "STRING" },
    { "name" : "provider", "type": "STRING" },
    { "name" : "recognized_lines", "type": "INT64" },
    { "name" : "total_lines", "type": "INT64" },
    { "name" : "unrecognized_lines", "type": "INT64" }
    ]
  },
  "sourceFormat": "NEWLINE_DELIMITED_JSON",
  "sourceUris": [
  "gs://logmon_logs_parsed_us/logs_splunk_public/v3/summary*" ]
}
EOF

jq -S -c -e . < cloudian_summary_schema.json > /dev/null
jq -S -c .schema.fields < cloudian_summary_schema.json > cloudian_summary_schema_only.json

bq rm -f strides_analytics.cloudian_summary
bq load \
    --source_format=NEWLINE_DELIMITED_JSON \
    strides_analytics.cloudian_summary \
        "gs://logmon_logs_parsed_us/logs_splunk_public/v3/summary.*" \
    cloudian_summary_schema_only.json

bq -q query \
--use_legacy_sql=false \
"select count(*) as summary_count from strides_analytics.cloudian_summary"

bq -q query --use_legacy_sql=false "SELECT log_date, parser_version, recognized_lines, unrecognized_lines FROM \`ncbi-logmon.strides_analytics.cloudian_summary\` where unrecognized_lines > 0 order by log_date"



echo " #### cloudian_parsed"
cat << EOF > cloudian_schema.json
    { "schema": { "fields": [
    { "name" : "accession", "type": "STRING" },
    { "name" : "bucket", "type": "STRING" },
    { "name" : "contentAccessorUserID", "type": "STRING" },
    { "name" : "copySource", "type": "STRING" },
    { "name" : "durationMsec", "type": "STRING" },
    { "name" : "eTag", "type": "STRING" },
    { "name" : "errorCode", "type": "STRING" },
    { "name" : "extension", "type": "STRING" },
    { "name" : "filename", "type": "STRING" },
    { "name" : "httpStatus", "type": "STRING" },
    { "name" : "ip", "type": "STRING" },
    { "name" : "method", "type": "STRING" },
    { "name" : "owner", "type": "STRING" },
    { "name" : "path", "type": "STRING" },
    { "name" : "reqId", "type": "STRING" },
    { "name" : "requestBodySize", "type": "STRING" },
    { "name" : "requestHdrSize", "type": "STRING" },
    { "name" : "responseBodySize", "type": "STRING" },
    { "name" : "responseHdrSize", "type": "STRING" },
    { "name" : "syslog_prefix", "type": "STRING" },
    { "name" : "timestamp", "type": "STRING" },
    { "name" : "totalSize", "type": "STRING" }
    ]
  },
  "sourceFormat": "NEWLINE_DELIMITED_JSON",
  "sourceUris": [
  "gs://logmon_logs_parsed_us/logs_splunk_public/v3/recognized*" ]
}
EOF

jq -S -c -e . < cloudian_schema.json > /dev/null
jq -S -c .schema.fields < cloudian_schema.json > cloudian_schema_only.json

gsutil ls -lR "gs://logmon_logs_parsed_us/logs_splunk_public/v3/recognized.splunk.*" \
    | head -3 || true
gsutil ls -lR "gs://logmon_logs_parsed_us/logs_splunk_public/v3/recognized.splunk.*" \
    | tail -3 || true

bq rm -f strides_analytics.cloudian_parsed
bq load \
    --source_format=NEWLINE_DELIMITED_JSON \
    strides_analytics.cloudian_parsed \
        "gs://logmon_logs_parsed_us/logs_splunk_public/v3/recognized.*" \
    cloudian_schema_only.json

bq show --schema strides_analytics.cloudian_parsed

bq -q query \
--use_legacy_sql=false \
"select count(*) as cloudian_parsed_count from strides_analytics.cloudian_parsed"


echo " #### cloudian_fixed"
    QUERY=$(cat <<-ENDOFQUERY
    SELECT
        accession,
        bucket,
        contentAccessorUserID,
        copySource,
        safe_cast(durationMsec as int64) as durationMsec,
        eTag,
        errorCode,
        extension,
        filename,
        safe_cast(httpStatus as int64) as httpStatus,
        ip,
        method,
        owner,
        path,
        reqId,
        safe_cast(requestBodySize as int64) as requestBodySize,
        safe_cast(requestHdrSize as int64) as requestHdrSize,
        safe_cast(responseBodySize as int64) as responseBodySize,
        safe_cast(responseHdrSize as int64) as responseHdrSize,
        syslog_prefix,
        timestamp_add(
            parse_timestamp(
                "%Y-%m-%d %H:%M:%S",
                split(timestamp, ",")[offset(0)] ) ,
                interval
                  cast (
                    split(timestamp, ",")[offset(1)]
                    as int64)
                  millisecond)
                as timestamp,
        safe_cast(totalSize as int64) as totalSize,
        current_datetime() as fixed_time
    FROM \\\`ncbi-logmon.strides_analytics.cloudian_parsed\\\`
ENDOFQUERY
)

# TODO Hack, cause I can't understand bash backtick quoting
QUERY="${QUERY//\\/}"
bq rm --project_id ncbi-logmon -f strides_analytics.cloudian_fixed
# shellcheck disable=SC2016
bq query \
--project_id ncbi-logmon \
--destination_table strides_analytics.cloudian_fixed \
--use_legacy_sql=false \
--batch=true \
--max_rows=5 \
"$QUERY"

bq show --schema strides_analytics.cloudian_fixed


bq -q query \
--use_legacy_sql=false \
"select durationMsec, errorCode, requestBodySize, requestHdrSize, requestBodySize, responseBodySize, responseHdrSize, totalSize from strides_analytics.cloudian_fixed where durationMsec is null or errorCode is null or requestBodySize is null or requestHdrSize is null or responseBodySize is null or responseHdrSize is null or totalSize is null"

date
