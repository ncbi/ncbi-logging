#!/bin/bash

# shellcheck source=/home/vartanianmh/strides/strides_env.sh
. "$HOME/strides/strides_env.sh"

mkdir -p "$LOGDIR"/sra_prod

for file in /panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive/*/*/*/*/access.log_????????.*.gz;
do
    ls -l "$file"
    outfile=$(echo "$file" | tr -d '/')
    outdate=$(echo "$outfile" | pcregrep -o "20[\d]{6}" | head -1)

    mkdir -p "$PANFS/sra_prod/$outdate"
    cp -v "$file" "$PANFS/sra_prod/$outdate/$outfile"
#    mkdir -p "$LOGDIR/sra_prod/$outdate"
#    cp -v "$file" "$LOGDIR/sra_prod/$outdate/$outfile"
done

#rm -f "$LOGDIR"/sra_prod/"$YESTERDAY"/*old*gz

zcat "$PANFS"/sra_prod/"$YESTERDAY"/*gz | \
    time "$HOME/strides/nginxtojson" 2> "$LOGDIR/sra_prod/$YESTERDAY.err" | \
    gzip -9 -c > "$LOGDIR/sra_prod/$YESTERDAY.jsonl.gz"



export GOOGLE_APPLICATION_CREDENTIALS=/home/vartanianmh/sandbox-blast-847af7ab431a.json
gcloud config set account 1008590670571-compute@developer.gserviceaccount.com
export CLOUDSDK_CORE_PROJECT="ncbi-sandbox-blast"


gsutil -m rsync -r "$LOGDIR"/sra_prod gs://strides_analytics/sra_prod

#chmod g+rx "$LOGDIR"
#chmod g+r "$LOGDIR/*"
#chmod -R o-rwx "$LOGDIR"

df -HT "$LOGDIR"

date
