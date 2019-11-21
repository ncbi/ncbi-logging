#!/bin/bash

# shellcheck source=/home/vartanianmh/strides/strides_env.sh
. "$HOME/strides/strides_env.sh"

mkdir -p "$PANFS/export"

cd "$PANFS/export" || exit
#rm -f export.*

export GOOGLE_APPLICATION_CREDENTIALS=/home/vartanianmh/sandbox-blast-847af7ab431a.json
gcloud config set account 1008590670571-compute@developer.gserviceaccount.com
export CLOUDSDK_CORE_PROJECT="ncbi-sandbox-blast"

gsutil du -s -h gs://strides_analytics/
gsutil du -s -h gs://strides_analytics/sra_prod/
gsutil du -s -h gs://strides_analytics/s3_prod/
gsutil du -s -h gs://strides_analytics/gs_prod/
gsutil du -s -h gs://strides_analytics/export/
gsutil du -s -h 'gs://strides_analytics/s3_prod/*jsonl.gz'
gsutil du -s -h 'gs://strides_analytics/sra_prod/*jsonl.gz'

gsutil -m rm -f "gs://strides_analytics/export/export.$DATE.*"

bq rm -f strides_analytics.export

bq show --schema strides_analytics.combined
bq show --view strides_analytics.combined

bq show --schema strides_analytics.gs_fix
bq show --view strides_analytics.gs_fix

bq show --schema strides_analytics.s3_fix
bq show --view strides_analytics.s3_fix

bq show --schema strides_analytics.sra_fix
bq show --view strides_analytics.sra_fix

bq show --schema strides_analytics.gs_prod
bq show --schema strides_analytics.s3_prod
bq show --schema strides_analytics.sra_prod

bq query \
    --destination_table strides_analytics.export \
    --use_legacy_sql=false \
    --batch=true \
    --max_rows=10 \
   'SELECT
  status,
  ip,
  domain,
  cmds,
  bytecount,
  agent,
  cnt,
  acc,
  start,
  `end`,
  source,
    IF (REGEXP_CONTAINS(ip, ":"), 0,
    NET.IPV4_TO_INT64(NET.SAFE_IP_FROM_STRING(ip)) ) AS ipint
  FROM
  `ncbi-sandbox-blast.strides_analytics.combined`
  WHERE regexp_contains(acc, "[DES]RR[\\.0-9]{6,13}")
  AND start > "2016-01-01" '


bq extract \
    --destination_format CSV \
    --compression GZIP \
    'strides_analytics.export' \
    "gs://strides_analytics/export/export.$DATE.*.csv.gz"

rm $PANFS/export/export."$DATE".*

gsutil -m cp -r "gs://strides_analytics/export/export.$DATE.*" "$PANFS/export/"

date
