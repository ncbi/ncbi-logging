#!/bin/bash

# shellcheck source=strides_env.sh
. ./strides_env.sh

export GOOGLE_APPLICATION_CREDENTIALS=/home/vartanianmh/sandbox-blast-847af7ab431a.json
gcloud config set account 1008590670571-compute@developer.gserviceaccount.com
export CLOUDSDK_CORE_PROJECT="ncbi-sandbox-blast"

bq rm -f strides_analytics.detail_export

bq show --schema strides_analytics.detail

# shellcheck disable=SC2016
bq query \
    --destination_table strides_analytics.detail_export \
    --use_legacy_sql=false \
    --batch=true \
    --max_rows=10 \
   'SELECT
  ip,
  start_ts,
  uri,
  status,
  bytecount,
  referer,
  agent,
  end_ts,
  host,
  cmd,
  bucket,
  source
  FROM
  `ncbi-sandbox-blast.strides_analytics.detail`
  '


bq extract \
    --destination_format JSON \
    --compression GZIP \
    'strides_analytics.detail_export' \
    "gs://strides_analytics/detail/detail.$DATE.*.json.gz"

mkdir -p "$PANFS/detail"
cd "$PANFS/detail" || exit

rm "$PANFS"/detail/detail."$DATE".*

gsutil -m cp -r "gs://strides_analytics/detail/detail.$DATE.*" "$PANFS/detail/"

date
