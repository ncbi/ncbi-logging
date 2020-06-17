#!/bin/bash

USAGE="Usage: $0 {S3,GS,OP} [YYYY_MM_DD]"

# shellcheck source=strides_env.sh
. ./strides_env.sh

case "$#" in
    0)
        echo "$USAGE"
        exit 1
        ;;
    1)
        PROVIDER=$1
        DATE_UNDER=$(date "+%Y_%m_%d") #_%H%
        ;;
    2)
        PROVIDER=$1
        DATE_UNDER=$2
        ;;
    *)
        echo "$USAGE"
        exit 1
        ;;
esac

PROVIDER_LC=${PROVIDER,,}
DATE=${DATE_UNDER//_}
DATE_DASH=${DATE_UNDER//_/-}

echo "DATE=$DATE DATE_UNDER=$DATE_UNDER DATE_DASH=$DATE_DASH"

case "$PROVIDER" in
    S3)
        export PARSER="aws"
        export MIRROR="$HOME/S3.all"
        ;;
    GS)
        export PARSER="gcp"
        MIRROR="$TMP/$PROVIDER/$LOG_BUCKET/mirror"
        ;;
    OP)
        export PARSER="op"
        ;;
    *)
        echo "Invalid provider $PROVIDER"
        echo "$USAGE"
        exit 1
esac


buckets=$(sqlcmd "select distinct log_bucket from buckets where cloud_provider='$PROVIDER' order by log_bucket desc")

echo "buckets is '$buckets'"
for LOG_BUCKET in $buckets; do
    echo "Processing $LOG_BUCKET"

    mkdir -p "$MIRROR"
    cd "$MIRROR" || exit

    PROFILE=$(sqlcmd "select service_account from buckets where cloud_provider='$PROVIDER' and bucket_name='$LOG_BUCKET'")
    echo "Profile is $PROFILE, $PROVIDER rsyncing to $MIRROR..."

    if [ "$PROVIDER" = "GS" ]; then
        TGZ="$PANFS/$PROVIDER/$LOG_BUCKET/$PROVIDER.$LOG_BUCKET.$DATE.tar.gz"
        export GOOGLE_APPLICATION_CREDENTIALS=/home/vartanianmh/nih-sra-datastore-c9b0ec6d9244.json
        export CLOUDSDK_CORE_PROJECT="nih-sra-datastore"
        gcloud auth activate-service-account --key-file=$GOOGLE_APPLICATION_CREDENTIALS

        gsutil -m rsync "gs://$LOG_BUCKET/" .
        WILDCARD="*_usage_${DATE_UNDER}_*v0"
    fi

    if [ "$PROVIDER" = "S3" ]; then
        TGZ="$HOME/$PROVIDER/$LOG_BUCKET/$PROVIDER.$LOG_BUCKET.$DATE.tar.gz"
#        if [ "$LOG_BUCKET" = "sra-pub-src-1-logs" ]; then
#            cp "$PANFS"/s3_prod/"$DATE".src.combine.gz .
#        else
#            cp "$PANFS"/s3_prod/"$DATE".combine.gz .
#        fi

        WILDCARD="${DATE_DASH}-*"
#        gsutil -m rsync "s3://$LOG_BUCKET/" .
#        gunzip ./*combine.gz
#        WILDCARD="*.combine"
    fi

    mkdir -p "$(dirname "$TGZ")"
    echo "rsynced to $MIRROR, tarring $WILDCARD to $TGZ ..."

    find . -name "$WILDCARD" -print0| tar -caf "$TGZ" --null --files-from -
    echo "Tarred"
    ls -l "$TGZ"

    DEST_BUCKET="gs://strides_analytics_logs_${PROVIDER_LC}_public/"

    export GOOGLE_APPLICATION_CREDENTIALS=$HOME/sandbox-blast-847af7ab431a.json
    gcloud config set account 1008590670571-compute@developer.gserviceaccount.com
    export CLOUDSDK_CORE_PROJECT="ncbi-sandbox-blast"

    echo "Copying $TGZ to $DEST_BUCKET"
    gsutil cp "$TGZ" "$DEST_BUCKET"
    echo "Done with $LOG_BUCKET"
done

#cd "$TMP"/GS || exit
#tar -czf "$PANFS"/GS."$DATE".tar.gz . &

echo "Done"
