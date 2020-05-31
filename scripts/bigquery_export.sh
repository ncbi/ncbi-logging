#!/bin/bash

# shellcheck source=strides_env.sh
. ./strides_env.sh

export GOOGLE_APPLICATION_CREDENTIALS=/home/vartanianmh/sandbox-blast-847af7ab431a.json
gcloud config set account 1008590670571-compute@developer.gserviceaccount.com
export CLOUDSDK_CORE_PROJECT="ncbi-sandbox-blast"

bq show --schema strides_analytics.s3_parsed

bq rm -f strides_analytics.detail_export

YESTERDAY="20200528"
DATE="20200529"

QUERY=$(cat <<-ENDOFQUERY
SELECT
  ip as remote_ip,
  parse_datetime('%d.%m.%Y:%H:%M:%S 0', time) as start_ts,
  datetime_add(parse_datetime('%d.%m.%Y:%H:%M:%S 0', time), interval total_time millisecond) as end_ts,
  case
      WHEN regexp_contains(agent, r'-head') THEN 'HEAD'
      ELSE split(request, ' ')[offset (0)]
      END as http_operation,
  split(request, ' ')[offset (1)] as request_uri,
  cast(res_code as string) as http_status,
  ifnull(res_len, 0) as bytes_sent,
  referer as referer,
  replace(agent, '-head', '') as user_agent,
  host_header as host,
  bucket as bucket,
  source as source,
  current_datetime() as export_time
  FROM \\\`ncbi-sandbox-blast.strides_analytics.s3_parsed\\\`
  WHERE
    parse_datetime('%d.%m.%Y:%H:%M:%S 0', time)
  between
    datetime_sub(parse_datetime('%Y%m%d', '$YESTERDAY'), interval 1 day) AND
    parse_datetime('%Y%m%d', '$YESTERDAY')
ENDOFQUERY
)

# TODO Hack, cause I can't understand bash backtick quoting
QUERY="${QUERY//\\/}"

echo "Query is $QUERY"

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
  FROM \\\`ncbi-sandbox-blast.strides_analytics.detail_export\\\`
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

bq extract \
    --destination_format NEWLINE_DELIMITED_JSON \
    --compression GZIP \
    'strides_analytics.detail_export' \
    "gs://strides_analytics/detail/detail.$DATE.*.json.gz"

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
gsutil ls -p ncbi-sandbox-blast "gs://strides_analytics/detail/detail.$DATE.*"
gsutil -m cp -r "gs://strides_analytics/detail/detail.$DATE.*" "$PANFS/detail/"

mkdir -p "$PANFS/summary"
cd "$PANFS/summary" || exit
rm -f "$PANFS"/summary/summary."$DATE".* || true
gsutil ls -p ncbi-sandbox-blast "gs://strides_analytics/summary/summary.$DATE.*"
gsutil -m cp -r "gs://strides_analytics/summary/summary.$DATE.*" "$PANFS/summary/"

date
