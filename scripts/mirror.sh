#!/bin/bash

USAGE="Usage: $0 {S3,GS,OP,Splunk} [YYYY_MM_DD [bucket]]"

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

mkdir -p "${HOME}/logs/"
LOGFILE="${HOME}/logs/mirror.${HOST}.${PROVIDER_LC}.${YESTERDAY}.log"
touch "$LOGFILE"
exec 1>"$LOGFILE"
exec 2>&1

echo "YESTERDAY=$YESTERDAY YESTERDAY_UNDER=$YESTERDAY_UNDER YESTERDAY_DASH=$YESTERDAY_DASH"

if [[ ${#YESTERDAY_UNDER} -ne 10 ]]; then
    echo "Invalid date: $YESTERDAY_UNDER"
    echo "$USAGE"
    exit 2
fi

case "$PROVIDER" in
    S3)
        ;;
    GS)
        ;;
    OP)
        ;;
    Splunk)
        ;;
    *)
        echo "Invalid provider $PROVIDER"
        echo "$USAGE"
        exit 1
esac


buckets=$(sqlcmd "select distinct log_bucket from buckets where scope='$STRIDES_SCOPE' and cloud_provider='$PROVIDER' order by log_bucket desc")
readarray -t buckets <<< "$buckets"
echo "buckets has ${#buckets[@]}, is ' " "${buckets[*]}" "'"
for LOG_BUCKET in "${buckets[@]}"; do
    echo sqlcmd "select service_account from buckets where cloud_provider='$PROVIDER' and bucket_name='$LOG_BUCKET'"
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
        if [ "$STRIDES_SCOPE" = "public" ]; then
            MIRROR="$TMP/$PROVIDER/$LOG_BUCKET"
        else
            MIRROR="$RAMDISK/$PROVIDER/$LOG_BUCKET"
        fi
        mkdir -p "$MIRROR"
        cd "$MIRROR" || exit

        export CLOUDSDK_CORE_PROJECT="nih-sra-datastore"
        gcloud config set account strides-analytics@nih-sra-datastore.iam.gserviceaccount.com

        if [ "$PROFILE" = "logmon_private" ]; then
            export CLOUDSDK_CORE_PROJECT="nih-sra-datastore-protected"
            export GOOGLE_APPLICATION_CREDENTIALS=/home/vartanianmh/logmon-nih-sra-datastore.json
            gcloud config set account logmon@nih-sra-datastore-protected.iam.gserviceaccount.com
        fi

        echo "Profile is $PROFILE, $PROVIDER rsyncing gs://$LOG_BUCKET to $MIRROR..."

        #gsutil -m rsync -x ".*_2019_.*|.*_2020_.*" "gs://$LOG_BUCKET/" .
        gsutil -m rsync -x ".*_2019_.*|.*_2020_.*|.*_2021_0.*" "gs://$LOG_BUCKET/" .
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
        MIRROR="$PANFS/s3_mirror/$PROVIDER/$LOG_BUCKET"
        mkdir -p "$MIRROR"
        cd "$MIRROR" || exit

        echo "Profile is $PROFILE, $PROVIDER concatenating to $MIRROR ..."

        if [ "$PROFILE" = "logmon_private" ]; then
            echo "Overriding AWS_PROFILE to nih-nlm-ncbi-sra-protected"
            export AWS_PROFILE="nih-nlm-ncbi-sra-protected"
        fi

        if [ "$LOG_BUCKET" = "sra-pub-src-1-logs" ]; then
            echo "Overriding AWS_PROFILE to opendata"
            export AWS_PROFILE="opendata"
        fi

        if [ "$LOG_BUCKET" = "sra-pub-run-1-logs" ]; then
            echo "Overriding AWS_PROFILE to strides-analytics"
            export AWS_PROFILE="strides-analytics"
        fi
        WILDCARD="${YESTERDAY_DASH}-*"

        for HH in $(seq 0 1 23); do
            PREFIX=$(printf "%s-%0.2d" "${YESTERDAY_DASH}" "$HH")
            echo "Concatenating $PREFIX..."
            echo "$HOME/ncbi-logging/scripts/s3_cat.py $LOG_BUCKET ${PREFIX}- $AWS_PROFILE"
            sleep 1
            "$HOME/ncbi-logging/scripts/s3_cat.py" "$LOG_BUCKET" "${PREFIX}-" "$AWS_PROFILE" > "$MIRROR"/"$PREFIX"-combine &
        done
        wait

        #aws s3 sync "s3://$LOG_BUCKET" . --exclude "*" --include "$WILDCARD" --quiet
    fi

    if [ "$PROVIDER" = "Splunk" ]; then
        MIRROR="$TMP/$PROVIDER/$LOG_BUCKET/"
        mkdir -p "$MIRROR"
        cd "$MIRROR" || exit

        echo "Profile is $PROFILE, $PROVIDER concatenating to $MIRROR ..."
        WILDCARD="${YESTERDAY_DASH}:*"

        # 2020_11_24 -> 11/1/2020:HH:MM:SS
        YYYY=$(echo "$YESTERDAY_DASH" | cut -d'-' -f 1)
        MM=$(echo "$YESTERDAY_DASH" | cut -d'-' -f 2)
        DD=$(echo "$YESTERDAY_DASH" | cut -d'-' -f 3)
        YESTERDAY_SLASH="$MM/$DD/$YYYY"

        for HH in $(seq -f "%02.0f" 0 1 23); do
            for MM in $(seq -f "%02.0f" 0 5 59); do
                HH_MM_FROM="$HH:${MM}:00"
                MM_END=$(( MM + 4 ))
                MM_END=$(printf "%02.0f" $MM_END)
                HH_MM_TO="$HH:${MM_END}:59"
                echo "From   $YESTERDAY_SLASH $HH_MM_FROM to $HH_MM_TO"
                "$HOME/ncbi-logging/parser/splunk/splunkreq.py" \
                    --bearer "$HOME/splunk_bearer.txt" \
                    --timeout 900 \
                    --earliest "$YESTERDAY_SLASH:$HH_MM_FROM" \
                    --latest   "$YESTERDAY_SLASH:$HH_MM_TO" \
                    > "$YESTERDAY_DASH:$HH_MM_FROM"
            done
        done
    fi

    TGZ="$YESTERDAY_DASH.$LOG_BUCKET.tar.gz"

    # ls -lh

    echo "rsynced to $MIRROR, tarring $WILDCARD to $TGZ ..."

    find . -name "$WILDCARD" -print0 | sort -z | tar -cavf "$TGZ" --null --files-from -
    echo "Tarred"
    ls -hl "$TGZ"

    if [ "$STRIDES_SCOPE" = "public" ]; then
        DEST_BUCKET="gs://logmon_logs/${PROVIDER_LC}_${STRIDES_SCOPE}/"
    fi

    if [ "$STRIDES_SCOPE" = "private" ]; then
        DEST_BUCKET="gs://logmon_logs_private/${PROVIDER_LC}_${STRIDES_SCOPE}/"
        export GOOGLE_APPLICATION_CREDENTIALS="$HOME/logmon_private.json"
    fi

    export CLOUDSDK_CORE_PROJECT="ncbi-logmon"
    gcloud config set account 253716305623-compute@developer.gserviceaccount.com

    echo "Copying $TGZ to $DEST_BUCKET"
    gsutil cp "$TGZ" "$DEST_BUCKET"

    if [ "$PROVIDER" != "GS" ] && [ "$PROVIDER" != "Splunk" ]; then
        cd ..
        echo "Deleting $MIRROR"
        df -HT "$MIRROR"
        rm -rf "$MIRROR"
    fi

    # cp "$TGZ" "$PANFS/$PROVIDER/"
    echo "Done with $LOG_BUCKET"
done

# gzip -9 -v "$LOGFILE"

echo "Done"
