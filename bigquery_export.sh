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
gsutil du -s -h gs://strides_analytics/export/
gsutil du -s -h gs://strides_analytics/gs_prod/
gsutil du -s -h gs://strides_analytics/s3_prod/
gsutil du -s -h gs://strides_analytics/sra_prod/
gsutil du -s -h 'gs://strides_analytics/s3_prod/*jsonl.gz'
gsutil du -s -h 'gs://strides_analytics/sra_prod/*jsonl.gz'

gsutil rm "gs://strides_analytics/export/export.$DATE.*"

bq extract \
    --destination_format CSV \
    --compression GZIP \
    'strides_analytics.export' \
    "gs://strides_analytics/export/export.$DATE.*.csv.gz"

rm $PANFS/export/export.$DATE.*

gsutil -m cp -r "gs://strides_analytics/export/export.$DATE.*" "$PANFS/export/"
