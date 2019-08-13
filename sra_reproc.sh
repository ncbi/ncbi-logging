#!/bin/bash

# shellcheck source=/home/vartanianmh/strides/strides_env.sh
. "$HOME/strides/strides_env.sh"

mkdir -p "$LOGDIR/sra_prod"

cd "$LOGDIR" || exit
#gsutil -m cp -r gs://strides_analytics/sra_prod .

cd "$LOGDIR/sra_prod" || exit


for file in /panfs/traces01.be-md.ncbi.nlm.nih.gov/sra_review/scratch/yaschenk/FTPLOGS/2018/*gz; do
    outfile=$(echo "$file" | tr -d '/')
    outdir=$(echo "$outfile" | pcregrep -o "20[\d]{6}" | head -1)
    outdir="$PANFS/sra_prod/$outdir"
    mkdir -p "$outdir"
    if [ ! -e "$outdir/$outfile" ]; then
       cp -n -v "$file" "$outdir/$outfile" &
    fi

#    gunzip -d -c "$dir"/*gz |
#        "$HOME/strides/nginxtojson" 2> "$dir.err" | \
#        gzip -9 -c > "$LOGDIR/sra_prod/$dir.jsonl.gz" &
#
#    gunzip -d -c $LOGDIR/sra_prod/"$YESTERDAY"/*gz | \
#    time "$HOME/strides/nginxtojson" 2> "$LOGDIR/sra_prod/$YESTERDAY.err" | \
#    gzip -9 -c > "$LOGDIR/sra_prod/$YESTERDAY.jsonl.gz"

    j=$(jobs | wc -l)
    while [ "$j" -ge 10 ]; do
        j=$(jobs | wc -l)
        sleep 1
    done
done

for dir in "$PANFS"/sra_prod/2018????; do
    outdir=$(echo "$dir" | pcregrep -o "20[\d]{6}" | head -1)
    echo "$outdir"
    gunzip -d -c "$dir"/*gz | time "$HOME"/strides/nginxtojson  2> \
        "$LOGDIR/sra_prod/$outdir.err" | \
        gzip -9 -c > "$LOGDIR/sra_prod/$outdir.jsonl.gz" &

    j=$(jobs | wc -l)
    while [ "$j" -ge 80 ]; do
        j=$(jobs | wc -l)
        sleep 1
    done


done

wait


