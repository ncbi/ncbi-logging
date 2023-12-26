#!/bin/bash

export CLOUDSDK_CORE_PROJECT="ncbi-logmon"

cd "$PANFS/s3_prod/objects" || exit
gcloud config set account 253716305623-compute@developer.gserviceaccount.com
gsutil -m cp -n ./*.gz gs://logmon_objects/s3/
#gsutil -m rsync . gs://logmon_objects/s3/
find "$PANFS/s3_prod/objects" -mtime +10 -delete

cd "$PANFS/gs_prod/objects" || exit
gcloud config set account 253716305623-compute@developer.gserviceaccount.com
gsutil -m cp -n ./*.gz gs://logmon_objects/gs/
#gsutil -m rsync . gs://logmon_objects/gs/
find "$PANFS/gs_prod/objects" -mtime +10 -delete

echo "Done"

exit 0
