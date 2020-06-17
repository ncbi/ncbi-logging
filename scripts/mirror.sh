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
        YESTERDAY_UNDER=$(date -d "yesterday" "+%Y_%m_%d") #_%H%
        ;;
    2)
        PROVIDER=$1
        YESTERDAY_UNDER=$2
        ;;
    *)
        echo "$USAGE"
        exit 1
        ;;
esac

PROVIDER_LC=${PROVIDER,,}
YESTERDAY=${YESTERDAY_UNDER//_}
YESTERDAY_DASH=${YESTERDAY_UNDER//_/-}

echo "YESTERDAY=$YESTERDAY YESTERDAY_UNDER=$YESTERDAY_UNDER YESTERDAY_DASH=$YESTERDAY_DASH"

case "$PROVIDER" in
    S3)
        export PARSER="aws"
        ;;
    GS)
        export PARSER="gcp"
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

    PROFILE=$(sqlcmd "select service_account from buckets where cloud_provider='$PROVIDER' and bucket_name='$LOG_BUCKET'")

    if [ "$PROVIDER" = "GS" ]; then
        MIRROR="$TMP/$PROVIDER/$LOG_BUCKET/mirror"
        mkdir -p "$MIRROR"
        cd "$MIRROR" || exit
        TGZ="$PANFS/$PROVIDER/$LOG_BUCKET/$PROVIDER.$LOG_BUCKET.$YESTERDAY.tar.gz"
        export GOOGLE_APPLICATION_CREDENTIALS=/home/vartanianmh/nih-sra-datastore-c9b0ec6d9244.json
        export CLOUDSDK_CORE_PROJECT="nih-sra-datastore"
        gcloud auth activate-service-account --key-file=$GOOGLE_APPLICATION_CREDENTIALS

        echo "Profile is $PROFILE, $PROVIDER rsyncing to $MIRROR..."
        gsutil -m rsync "gs://$LOG_BUCKET/" .
        WILDCARD="*_usage_${YESTERDAY_UNDER}_*v0"
    fi

    if [ "$PROVIDER" = "S3" ]; then
        MIRROR="$HOME/S3.all"
        mkdir -p "$MIRROR"
        cd "$MIRROR" || exit
        TGZ="$HOME/$PROVIDER/$LOG_BUCKET/$PROVIDER.$LOG_BUCKET.$YESTERDAY.tar.gz"
#        if [ "$LOG_BUCKET" = "sra-pub-src-1-logs" ]; then
#            cp "$PANFS"/s3_prod/"$YESTERDAY".src.combine.gz .
#        else
#            cp "$PANFS"/s3_prod/"$YESTERDAY".combine.gz .
#        fi

        echo "Profile is $PROFILE, $PROVIDER rsyncing to $MIRROR..."
        WILDCARD="${YESTERDAY_DASH}-*"
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
#tar -czf "$PANFS"/GS."$YESTERDAY".tar.gz . &

echo "Done"
