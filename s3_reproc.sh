#!/bin/bash

# shellcheck source=/home/vartanianmh/strides/strides_env.sh
. "$HOME/strides/strides_env.sh"

cd $LOGDIR/s3_prod || exit

for x in *.combine.gz; do
    d=${x:0:8}
    echo $d

    zcat $d.combine.gz | \
    "$HOME/strides/s3tojson" 2> "$LOGDIR/s3_prod/$d.err" | \
    gzip -9 -c > "$LOGDIR/s3_prod/$d.jsonl.gz" &

    j=$(jobs | wc -l)

    while [ "$j" -ge 10 ]; do
        j=$(jobs | wc -l)
        sleep 5
    done
done

wait
