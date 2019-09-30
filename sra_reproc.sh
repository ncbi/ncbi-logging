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

for outdir in 20181109 20181221 20190101 20190213 20190220 20190228 20190301 20190307 20190311 20190314 20190330 20190403 20190404 20190405 20190406 20190410 20190413; do


for dir in "$PANFS"/sra_prod/201?????; do
    outdir=$(echo "$dir" | pcregrep -o "20[\d]{6}" | head -1)
    echo "$outdir"
    # md5sum "$PANFS/sra_prod/$outdir/"*gz > /tmp/mike_logs/sra_prod/md5s/$outdir.md5 &
    gunzip -d -c "$PANFS/sra_prod/$outdir/"*gz | "$HOME"/strides/nginxtojson  2> \
        "$LOGDIR/sra_prod/$outdir.err.2" | \
        gzip -9 -c > "$LOGDIR/sra_prod/$outdir.jsonl.gz.2" &

    j=$(jobs | wc -l)
    while [ "$j" -ge 30 ]; do
        j=$(jobs | wc -l)
        sleep 5
    done
done

wait


