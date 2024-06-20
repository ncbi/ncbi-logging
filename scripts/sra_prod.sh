#!/bin/bash

# shellcheck source=/home/vartanianmh/strides/strides_env.sh
. ./strides_env.sh

#for file in /panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive/*/*/*/*/access.log_????????.*.gz;
for file in /netmnt/vast01/adb/logarchive/ftp.http/local_archive/*/*/*/*/access_log_????????.*.gz;
do
    ls -l "$file"
    outfile=$(echo "$file" | tr -d '/')
    outdate=$(echo "$outfile" | pcregrep -o "20[\d]{6}" | head -1)
#    echo "Processing $outdate"

    mkdir -p "$PANFS/sra_prod/$outdate"
    cp -n -v "$file" "$PANFS/sra_prod/$outdate/$outfile"
#    mkdir -p "$LOGDIR/sra_prod/$outdate"
#    cp -v "$file" "$LOGDIR/sra_prod/$outdate/$outfile"
done

date

exit 0
