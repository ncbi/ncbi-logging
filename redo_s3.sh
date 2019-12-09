#!/bin/bash

# shellcheck source=/home/vartanianmh/strides/strides_env.sh
. "$HOME/strides/strides_env.sh"

# NOTES:
# * aws s3 ls is much faster than other methods on buckets with
#   huge numbers of objects
# * We copy to ramdisk to avoid seeks, since we have thousands of small files

for days in $(seq 24); do
    DATE=$(date -d "2019-11-11 +$days days" +"%Y%m%d")
    YESTERDAY_DASH=$(date -d "2019-11-10 +$days days" +"%Y-%m-%d")
    YESTERDAY=${YESTERDAY_DASH//-}

    echo "DATE is $DATE, YESTERDAY_DASH is $YESTERDAY_DASH, YESTERDAY is $YESTERDAY"

    rm -f "$LOGDIR/s3_prod/$YESTERDAY.combine*"

    # VDB-3892 #for x in $(seq 7); do
    LOG_BUCKET="sra-pub-run-1-logs"
    DEST="$RAMDISK/S3-$LOG_BUCKET/$YESTERDAY"
    mkdir -p "$DEST"

    type -a aws

    # NOTE: Much faster to use aws ls than aws cp with include/excludes
    echo "Copying $LOG_BUCKET into $DEST ..."
    time aws s3 ls "s3://$LOG_BUCKET/$YESTERDAY_DASH" | cut -c 32- | \
        xargs -I % -P 12 aws s3 cp "s3://$LOG_BUCKET/%" "$DEST/%" --quiet
    date
    echo "Copied  $LOG_BUCKET  to  $DEST"

    cd "$DEST" || exit
    numfiles=$(find ./ -type f | wc -l)
    echo "Combining $numfiles files ..."
    unset combined
    declare -a combined
    for file in ./"$YESTERDAY_DASH"*; do
        combined+=("$file")
        if [[ "${#combined[@]}" -gt 20 ]]; then
            cat "${combined[@]}" >> "$LOGDIR/s3_prod/$YESTERDAY.combine"
            unset combined
            declare -a combined
        fi
    done
    date
    echo "Combined"

    cd "$LOGDIR" || exit
    echo "Processed  $LOG_BUCKET ..."
    rm -rf "$DEST"
    echo "Removed $DEST"

    time "$HOME/strides/s3tojson" < \
        "$LOGDIR/s3_prod/$YESTERDAY.combine" \
        2> "$LOGDIR/s3_prod/$YESTERDAY.err" | \
        gzip -9 -c > "$LOGDIR/s3_prod/$YESTERDAY.jsonl.gz" \

    time gzip -f -9 "$LOGDIR/s3_prod/$YESTERDAY.combine"
done
