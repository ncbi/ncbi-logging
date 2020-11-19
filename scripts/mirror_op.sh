#!/bin/bash

# shellcheck source=strides_env.sh
. ./strides_env.sh

#YESTERDAY="20200605"
# /panfs/traces01.be-md.ncbi.nlm.nih.gov/strides-analytics/sra_prod/20200607
# gsutil -m rsync -r "$LOGDIR"/sra_prod gs://strides_analytics/sra_prod
buckets=$(sqlcmd "select distinct log_bucket from buckets where cloud_provider='GS' and scope='public' order by log_bucket desc")
buckets="sra_prod"
echo "buckets is '$buckets'"
for LOG_BUCKET in $buckets; do
    echo "Processing $LOG_BUCKET"


    export GOOGLE_APPLICATION_CREDENTIALS=$HOME/sandbox-blast-847af7ab431a.json
    gcloud config set account 1008590670571-compute@developer.gserviceaccount.com
    export CLOUDSDK_CORE_PROJECT="ncbi-sandbox-blast"

    gsutil -m cp "$PANFS/sra_prod/$YESTERDAY/*gz" "gs://strides_analytics_logs_nginx_public/$YESTERDAY/"
#    gsutil -m rsync -r "$PANFS/sra_prod/$YESTERDAY/" "gs://strides_analytics_logs_nginx_public/$YESTERDAY/"

    echo
done

echo "Done"
