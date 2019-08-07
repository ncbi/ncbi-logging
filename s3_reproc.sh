#!/bin/bash

# shellcheck source=/home/vartanianmh/strides/strides_env.sh
. "$HOME/strides/strides_env.sh"

cd $LOGDIR/s3_prod || exit

for x in *.out.gz; do
    d=${x:0:8}
    gunzip -d -c "$d.out.gz" |  \
        "$HOME/strides/s3tojson" | \
        gzip -9 -c > "$LOGDIR/s3_prod/$d.jsonl.gz" &

    j=$(jobs | wc -l)

    while [ "$j" -ge 60 ]; do
        j=$(jobs | wc -l)
        sleep 1
    done
done

wait
