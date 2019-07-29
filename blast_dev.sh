#!/bin/bash

. $HOME/strides/strides_env.sh

export GOOGLE_APPLICATION_CREDENTIALS=/home/vartanianmh/sandbox-blast-847af7ab431a.json
gcloud config set account 1008590670571-compute@developer.gserviceaccount.com
export CLOUDSDK_CORE_PROJECT="ncbi-sandbox-blast"
LOG_BUCKET="yan-blastdb-access-logs"
mkdir -p /tmp/mike_logs/$LOG_BUCKET
gsutil -m rsync -r gs://$LOG_BUCKET /tmp/mike_logs/$LOG_BUCKET
~/strides/gs_agent_all.py $LOG_BUCKET 'blast dev'

./gs_storage.sh gs://yan-blastdb 'blast dev'


export GOOGLE_APPLICATION_CREDENTIALS=/home/vartanianmh/strides-blast-bffcaa7e6549.json
gcloud config set account strides-blast-bucket-ro@strides-blast.iam.gserviceaccount.com
LOG_BUCKET="blast-db-logs"
mkdir -p /tmp/mike_logs/$LOG_BUCKET
gsutil -m rsync -r gs://$LOG_BUCKET /tmp/mike_logs/$LOG_BUCKET
~/strides/gs_agent_all.py  $LOG_BUCKET blast




date
