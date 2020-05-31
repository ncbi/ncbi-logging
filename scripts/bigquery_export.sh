#!/bin/bash

# shellcheck source=strides_env.sh
. ./strides_env.sh

export GOOGLE_APPLICATION_CREDENTIALS=/home/vartanianmh/sandbox-blast-847af7ab431a.json
gcloud config set account 1008590670571-compute@developer.gserviceaccount.com
export CLOUDSDK_CORE_PROJECT="ncbi-sandbox-blast"

bq show --schema strides_analytics.s3_parsed

bq rm -f strides_analytics.detail_export

#YESTERDAY="20200529"
#DATE="20200530"

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
  res_code as http_status,
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
    --max_rows=10 \
    "$QUERY"

bq show --schema strides_analytics.s3_parsed

bq extract \
    --destination_format NEWLINE_DELIMITED_JSON \
    --compression GZIP \
    'strides_analytics.detail_export' \
    "gs://strides_analytics/detail/detail.$DATE.*.json.gz"

bq rm -f strides_analytics.detail_export

gsutil ls -p ncbi-sandbox-blast "gs://strides_analytics/detail/detail.$DATE.*"

mkdir -p "$PANFS/detail"
cd "$PANFS/detail" || exit
rm -f "$PANFS"/detail/detail."$DATE".* || true

gsutil -m cp -r "gs://strides_analytics/detail/detail.$DATE.*" "$PANFS/detail/"

date
