#!/bin/bash

# shellcheck source=/home/vartanianmh/strides/strides_env.sh
. "$HOME/strides/strides_env.sh"

export GOOGLE_APPLICATION_CREDENTIALS=/home/vartanianmh/nih-sra-datastore-c9b0ec6d9244.json
export CLOUDSDK_CORE_PROJECT="nih-sra-datastore"
gcloud auth activate-service-account --key-file=$GOOGLE_APPLICATION_CREDENTIALS

for LOG_BUCKET in "sra-pub-logs-1" "sra-ca-logs-1"; do
    echo "Processing $LOG_BUCKET"
    mkdir -p "$PANFS/gs_prod/$YESTERDAY"
    cd "$PANFS/gs_prod/$YESTERDAY" || exit
    gsutil -m cp "gs://$LOG_BUCKET/*_$YESTERDAY_UNDER*_v0" .

    find ./ -name "*_v0" -exec gzip -9 -f {} \;
    echo "Processed  $LOG_BUCKET"
done

export GOOGLE_APPLICATION_CREDENTIALS=/home/vartanianmh/requester-pays-key.json
export CLOUDSDK_CORE_PROJECT="research-sra-cloud-pipeline"
gcloud auth activate-service-account --key-file=$GOOGLE_APPLICATION_CREDENTIALS

mkdir -p "$PANFS/gs_prod/objects"
for x in $(seq 7); do
    "$HOME/strides/gs_lister.py" "sra-pub-run-$x" | \
        gzip -9 -c > \
        "$PANFS/gs_prod/objects/$DATE.objects-$x.gz"
done


export GOOGLE_APPLICATION_CREDENTIALS=/home/vartanianmh/sandbox-blast-847af7ab431a.json
gcloud config set account 1008590670571-compute@developer.gserviceaccount.com
export CLOUDSDK_CORE_PROJECT="ncbi-sandbox-blast"


gsutil -m rsync -r "$PANFS/gs_prod"  gs://strides_analytics/gs_prod

#rsync -av "$LOGDIR"/gs_prod/ "$PANFS/gs_prod/"

echo "Done"
date
