#!/bin/bash

# shellcheck source=/home/vartanianmh/strides/strides_env.sh
. "$HOME/strides/strides_env.sh"

mkdir -p "$LOGDIR/sra_prod"

cd "$LOGDIR" || exit
#gsutil -m cp -r gs://strides_analytics/sra_prod .

cd "$LOGDIR/sra_prod" || exit

dirs=`ls -1 $PANFS/restore/ | pcregrep -o "201....." | sort -u`

cd $PANFS/restore
for outdir in $dirs; do
    zcat *.access.log_*$outdir*gz  | \
        "$HOME"/strides/nginxtojson  2> \
        "$outdir.err" | \
        gzip -9 -c > "$outdir.jsonl.gz" &

    j=$(jobs | wc -l)
    while [ "$j" -ge 100 ]; do
        j=$(jobs | wc -l)
        sleep 1
    done
done

#for file in $PANFS/restore/*gz; do
#for file in /panfs/traces01.be-md.ncbi.nlm.nih.gov/sra_review/scratch/yaschenk/FTPLOGS/2018/*gz; do
    outfile=$(echo "$file" | tr -d '/')
    outdir=$(echo "$outfile" | pcregrep -o "20[\d]{6}" | head -1)
    outdir="$PANFS/sra_prod/$outdir"
#    mkdir -p "$outdir"
#    if [ ! -e "$outdir/$outfile" ]; then
#       cp -n -v "$file" "$outdir/$outfile" &
#    fi

    zcat 
#    zcat "$dir"/*gz |
#        "$HOME/strides/nginxtojson" 2> "$dir.err" | \
#        gzip -9 -c > "$LOGDIR/sra_prod/$dir.jsonl.gz" &
#
#    zcat $LOGDIR/sra_prod/"$YESTERDAY"/*gz | \
#    time "$HOME/strides/nginxtojson" 2> "$LOGDIR/sra_prod/$YESTERDAY.err" | \
#    gzip -9 -c > "$LOGDIR/sra_prod/$YESTERDAY.jsonl.gz"

done

for outdir in 20181109 20181221 20190101 20190213 20190220 20190228 20190301 20190307 20190311 20190314 20190330 20190403 20190404 20190405 20190406 20190410 20190413; do


for dir in "$PANFS"/sra_prod/201?????; do
    outdir=$(echo "$dir" | pcregrep -o "20[\d]{6}" | head -1)
    echo "$outdir"
    # md5sum "$PANFS/sra_prod/$outdir/"*gz > $LOGDIR/sra_prod/md5s/$outdir.md5 &
    zcat "$PANFS/sra_prod/$outdir/"*gz | \
        "$HOME"/strides/nginxtojson  2> \
        "$LOGDIR/sra_prod/$outdir.err" | \
        gzip -9 -c > "$LOGDIR/sra_prod/$outdir.jsonl.gz" &

    j=$(jobs | wc -l)
    while [ "$j" -ge 30 ]; do
        j=$(jobs | wc -l)
        sleep 5
    done
done

wait


