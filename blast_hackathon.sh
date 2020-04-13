#!/bin/bash

# shellcheck source=/home/vartanianmh/strides/strides_env.sh
. "$HOME/strides/strides_env.sh"

export GOOGLE_APPLICATION_CREDENTIALS=/home/vartanianmh/strides-service-account-file.json
gcloud config set account strides-data-bucket-log-reader@strides-sra-hackathon-data.iam.gserviceaccount.com
export CLOUDSDK_CORE_PROJECT="strides-sra-hackathon-data"
LOG_BUCKET="strides-hackathon-logs"
~/strides/query_stats.py > "bigqueries.sql.$DATE"
~/strides/gs_agent_all.py $LOG_BUCKET 'blast hackathon'

date
