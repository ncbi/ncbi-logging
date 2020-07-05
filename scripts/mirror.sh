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


buckets=$(sqlcmd "select distinct log_bucket from buckets where scope='public' and cloud_provider='$PROVIDER' order by log_bucket desc")

echo "buckets is '$buckets'"
for LOG_BUCKET in $buckets; do
    echo "Processing $LOG_BUCKET"

    PROFILE=$(sqlcmd "select service_account from buckets where cloud_provider='$PROVIDER' and bucket_name='$LOG_BUCKET'")

    if [ "$PROVIDER" = "GS" ]; then
        MIRROR="$TMP/$PROVIDER/$LOG_BUCKET/mirror"
        mkdir -p "$MIRROR"
        cd "$MIRROR" || exit

        export CLOUDSDK_CORE_PROJECT="nih-sra-datastore"
        gcloud config set account strides-analytics@nih-sra-datastore.iam.gserviceaccount.com

        echo "Profile is $PROFILE, $PROVIDER rsyncing to $MIRROR..."
        gsutil -m rsync "gs://$LOG_BUCKET/" .
        WILDCARD="*_usage_${YESTERDAY_UNDER}_*v0"
    fi

    if [ "$PROVIDER" = "OP" ]; then
        if [ "$YESTERDAY" -lt "20180701" ]; then
            files=$(find "$PANFS/restore" -type f -name "*$YESTERDAY*")
        elif [ "$YESTERDAY" -gt "20200701" ]; then
            files=$(find "$LOG_BUCKET" -type f -name "*$YESTERDAY*")
        else
            files=$(find "$PANFS/sra_prod/$YESTERDAY" -type f -name "*$YESTERDAY*")
        fi
        LOG_BUCKET="OP"
        MIRROR="$TMP/$PROVIDER/$LOG_BUCKET/"
        mkdir -p "$MIRROR"
        cd "$MIRROR" || exit
        for x in $files; do
            gunzip "$x"
        done

        echo "Profile is $PROFILE, $PROVIDER rsyncing to $MIRROR..."
    fi

    if [ "$PROVIDER" = "S3" ]; then
        MIRROR="$TMP/$PROVIDER/$LOG_BUCKET/"
        mkdir -p "$MIRROR"
        cd "$MIRROR" || exit

        echo "Profile is $PROFILE, $PROVIDER rsyncing to $MIRROR..."
        if [ "$LOG_BUCKET" = "sra-pub-src-1-logs" ]; then
            export AWS_PROFILE="opendata"
        fi

        if [ "$LOG_BUCKET" = "sra-pub-run-1-logs" ]; then
            export AWS_PROFILE="strides-analytics"
        fi
        WILDCARD="${YESTERDAY_DASH}-*"

        aws s3 sync "s3://$LOG_BUCKET" . --exclude "*" --include "$WILDCARD" --quiet
    fi

    TGZ="$YESTERDAY_DASH.$LOG_BUCKET.tar.gz"

    echo "rsynced to $MIRROR, tarring $WILDCARD to $TGZ ..."

    find . -name "$WILDCARD" -print0 | tar -caf "$TGZ" --null --files-from -
    echo "Tarred"
    ls -l "$TGZ"

    DEST_BUCKET="gs://logmon_logs/${PROVIDER_LC}_public/"

    export CLOUDSDK_CORE_PROJECT="ncbi-logmon"
    gcloud config set account 253716305623-compute@developer.gserviceaccount.com

    echo "Copying $TGZ to $DEST_BUCKET"
    gsutil cp "$TGZ" "$DEST_BUCKET"
    # cp "$TGZ" "$PANFS/$PROVIDER/"
    echo "Done with $LOG_BUCKET"
done

echo "Done"
