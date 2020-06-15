#!/bin/bash

export CLOUDSDK_CORE_PROJECT="ncbi-logmon"

cd "$PANFS/s3_prod/objects" || exit
gcloud config set account 253716305623-compute@developer.gserviceaccount.com
gsutil -m cp -n ./*.gz gs://logmon_objects/s3/
#gsutil -m rsync . gs://logmon_objects/s3/


cd "$PANFS/gs_prod/objects" || exit
gcloud config set account 253716305623-compute@developer.gserviceaccount.com
gsutil -m cp -n ./*.gz gs://logmon_objects/gs/
#gsutil -m rsync . gs://logmon_objects/gs/

exit 0
