#!/bin/bash

# shellcheck source=/home/vartanianmh/strides/strides_env.sh
. "$HOME/strides/strides_env.sh"

YESTERDAY_DASH=$(date -d "yesterday" "+%Y-%m-%d")
YESTERDAY_DASH="2019-07-14"
    YESTERDAY=${YESTERDAY_DASH//-}

    # NOTES:
    # * aws s3 ls is much faster than other methods on buckets with
    #   huge numbers of objects
    # * We copy to ramdisk to avoid seeks, since we have thousands of small files

    for x in $(seq 7); do
        LOG_BUCKET="sra-pub-run-$x-logs"
        DEST="$RAMDISK/S3-$LOG_BUCKET/$YESTERDAY"
        mkdir -p "$DEST"

        echo "Processing  $LOG_BUCKET into $DEST"
        aws s3 ls "s3://$LOG_BUCKET/$YESTERDAY_DASH" | cut -c 32- | \
            xargs -I % -P 24 aws s3 cp "s3://$LOG_BUCKET/%" "$DEST/%" --quiet

        cd "$DEST" || exit
        for file in ./*; do
            cat "$file" >> "$LOGDIR/s3_prod/$YESTERDAY.combine"
        done
        cd "$LOGDIR" || exit
        echo "Processed  $LOG_BUCKET"
        rm -rf "$DEST"
        echo "Removed $DEST"
    done

    "$HOME/strides/s3tojson" < \
        "$LOGDIR/s3_prod/$YESTERDAY.combine" | \
        gzip -9 -v -c > "$LOGDIR/s3_prod/$YESTERDAY.jsonl.gz"

    gzip -9 -v "$LOGDIR/s3_prod/$YESTERDAY.combine" &

