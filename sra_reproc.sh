#!/bin/bash

# shellcheck source=/home/vartanianmh/strides/strides_env.sh
. "$HOME/strides/strides_env.sh"

mkdir -p "$LOGDIR/sra_prod"

cd "$LOGDIR" || exit
#gsutil -m cp -r gs://strides_analytics/sra_prod .

cd "$LOGDIR/sra_prod" || exit


for dir in 201?????; do
#    d=${dir:0:8}
    gunzip -d -c "$dir"/*gz |
        "$HOME/strides/nginxtojson" 2> "$dir.err" | \
        gzip -9 -c > "$LOGDIR/sra_prod/$dir.jsonl.gz" &

    j=$(jobs | wc -l)

    while [ "$j" -ge 110 ]; do
        j=$(jobs | wc -l)
        sleep 10
    done
done

wait
