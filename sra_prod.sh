#!/bin/bash

# shellcheck source=/home/vartanianmh/strides/strides_env.sh
. "$HOME/strides/strides_env.sh"

mkdir -p /tmp/mike_logs/sra_prod

for file in /panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive/*/*/*/*/access.log_????????.*.gz;
do
    outfile=$(echo "$file" | tr -d '/')
    cp -n -v "$file" "/panfs/traces01.be-md.ncbi.nlm.nih.gov/strides-analytics/sra_prod/$outfile"

    outdir=`echo "$outfile" | pcregrep -o "20[\d]{6}" | head -1`
    outdir="/tmp/mike_logs/sra_prod/$outdir"
    mkdir -p "$outdir"
    if [ ! -e "$outdir/$outfile" ]; then
       cp -n -v "$file" "$outdir/$outfile"
    fi
done

gunzip -d -c $LOGDIR/sra_prod/"$YESTERDAY"/*gz | \
    time "$HOME/strides/nginxtojson" 2> "$LOGDIR/sra_prod/$YESTERDAY.err" | \
    gzip -9 -v -c > "$LOGDIR/sra_prod/$YESTERDAY.jsonl.gz"



export GOOGLE_APPLICATION_CREDENTIALS=/home/vartanianmh/sandbox-blast-847af7ab431a.json
gcloud config set account 1008590670571-compute@developer.gserviceaccount.com
export CLOUDSDK_CORE_PROJECT="ncbi-sandbox-blast"


gsutil -m rsync -r /tmp/mike_logs/sra_prod gs://strides_analytics/sra_prod

#rsync -av /tmp/mike_logs/sra_prod/ \
#    /panfs/traces01.be-md.ncbi.nlm.nih.gov/strides-analytics/sra_prod/



chmod g+rx /tmp/mike_logs
chmod g+r /tmp/mike_logs/*
chmod -R o-rwx /tmp/mike_logs

df -HT /tmp/mike_logs

date
