#!/bin/bash

USAGE="Usage: $0 {S3,GS,OP} [YYYY_MM_DD [bucket]]"

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
        BUCKET_ONLY=""
        ;;
    2)
        PROVIDER=$1
        YESTERDAY_UNDER=$2
        BUCKET_ONLY=""
        ;;
    3)
        PROVIDER=$1
        YESTERDAY_UNDER=$2
        BUCKET_ONLY=$3
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

if [[ ${#YESTERDAY_UNDER} -ne 10 ]]; then
    echo "Invalid date: $YESTERDAY_UNDER"
    echo "$USAGE"
    exit 2
fi

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
readarray -t buckets <<< "$buckets"
echo "buckets has ${#buckets[@]}, is ' " "${buckets[*]}" "'"
for LOG_BUCKET in "${buckets[@]}"; do
    PROFILE=$(sqlcmd "select service_account from buckets where cloud_provider='$PROVIDER' and bucket_name='$LOG_BUCKET'")
    BUCKET_NAME=$(sqlcmd "select distinct bucket_name from buckets where cloud_provider='$PROVIDER' and log_bucket='$LOG_BUCKET' limit 1")
    if [ -n "$BUCKET_ONLY" ]; then
        if [ "$BUCKET_NAME" != "$BUCKET_ONLY" ]; then
            echo "Skipping $BUCKET_NAME != $BUCKET_ONLY"
            continue
        fi
    fi

    echo "Processing $LOG_BUCKET"
    echo "BUCKET_NAME is $BUCKET_NAME"

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
#        if [[ "$LOG_BUCKET" =~ "srafiles" ]]; then
#            BUCKET_NAME="srafiles"
#        fi

        if [ "$YESTERDAY" -lt "20180701" ]; then
            files=$(find "$PANFS/restore" -type f -name "*$YESTERDAY*")
        elif [ "$YESTERDAY" -gt "20200706" ]; then
            files=""
            echo "Recent, LOG_BUCKET=$LOG_BUCKET"
            for f in $LOG_BUCKET; do
                if [[ $f == *"$YESTERDAY"* ]] || [[ $f == *"$YESTERDAY_DASH"* ]] || [[ $f == *"$YESTERDAY_UNDER"* ]] ; then
                    files="$files $f"
                fi
            done
#            files=$(/bin/ls -1 $LOG_BUCKET)
#            files=$(find $LOG_BUCKET -type f)
        else
            files=$(find "$PANFS/sra_prod/$YESTERDAY" -type f -name "*$YESTERDAY*")
        fi
        echo "files is $files"
        LOG_BUCKET="OP-${BUCKET_NAME}"
        MIRROR="$TMP/$PROVIDER/$LOG_BUCKET/$YESTERDAY"
        mkdir -p "$MIRROR"
        echo "Profile is $PROFILE, $PROVIDER copying to $MIRROR for $LOG_BUCKET..."

        cd "$MIRROR" || exit
        for x in $files; do
            newfile=$(echo "$x" | tr '/' '+')
            newfile=${newfile%".gz"}
#            if [ -s "$newfile" ]; then
#                continue
#            fi
            echo "  $x -> $newfile"
            zcat "$x" > "$newfile" || true # Some files are corrupt, continue
        done
        WILDCARD="+*"
    fi

    if [ "$PROVIDER" = "S3" ]; then
        MIRROR="$TMP/$PROVIDER/$LOG_BUCKET/"
        mkdir -p "$MIRROR"
        cd "$MIRROR" || exit

        echo "Profile is $PROFILE, $PROVIDER concatenating to $MIRROR ..."
        if [ "$LOG_BUCKET" = "sra-pub-src-1-logs" ]; then
            export AWS_PROFILE="opendata"
        fi

        if [ "$LOG_BUCKET" = "sra-pub-run-1-logs" ]; then
            export AWS_PROFILE="strides-analytics"
        fi
        WILDCARD="${YESTERDAY_DASH}-*"

        for HH in $(seq 0 1 23); do
            PREFIX=$(printf "%s-%0.2d" "${YESTERDAY_DASH}" "$HH")
            echo "Concatenating $PREFIX..."
            "$HOME/ncbi-logging/scripts/s3_cat.py" "$LOG_BUCKET" "${PREFIX}-" "$AWS_PROFILE" > "$MIRROR"/"$PREFIX"-combine &
        done
        wait

        #aws s3 sync "s3://$LOG_BUCKET" . --exclude "*" --include "$WILDCARD" --quiet
    fi

    TGZ="$YESTERDAY_DASH.$LOG_BUCKET.tar.gz"

    # ls -lh

    echo "rsynced to $MIRROR, tarring $WILDCARD to $TGZ ..."

    find . -name "$WILDCARD" -print0 | sort -z | tar -cavf "$TGZ" --null --files-from -
    echo "Tarred"
    ls -hl "$TGZ"

    DEST_BUCKET="gs://logmon_logs/${PROVIDER_LC}_public/"

    export CLOUDSDK_CORE_PROJECT="ncbi-logmon"
    gcloud config set account 253716305623-compute@developer.gserviceaccount.com

    echo "Copying $TGZ to $DEST_BUCKET"
    gsutil cp "$TGZ" "$DEST_BUCKET"

    if [ "$PROVIDER" != "GS" ]; then
        cd ..
        rm -rf "$MIRROR"
    fi

    # cp "$TGZ" "$PANFS/$PROVIDER/"
    echo "Done with $LOG_BUCKET"
done

echo "Done"
