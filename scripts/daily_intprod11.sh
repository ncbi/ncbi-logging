#!/bin/bash

echo "    --- Daily Processing Beginning ($$) ---"

umask u=rwx,g=r,o=

cd "$HOME/ncbi-logging/scripts" || exit

# shellcheck source=strides_env.sh
. ./strides_env.sh

set +e

mkdir -p "$HOME"/logs
find "$HOME/logs" -mtime +2 -size +1M -exec xz -9 {} \;

#/opt/panfs/bin/pan_df -H /panfs/traces01.be-md.ncbi.nlm.nih.gov/strides-analytics/

homespace=$( df  "$HOME" | tail  -1 | tr -s ' ' | cut -d ' ' -f 4)
if [ "$homespace" -lt 5000000 ]; then
    df -HT "$HOME" | mailx -s "$HOST $HOME low on space" vartanianmh@ncbi.nlm.nih.gov
fi

echo "mirror.sh GS"
./mirror.sh GS |& ts >> "$HOME"/logs/mirror_gs."$DATE".log

echo "mirror.sh OP"
./mirror.sh OP |& ts >> "$HOME"/logs/mirror_op."$DATE".log

echo "mirror.sh S3"
./mirror.sh S3 |& ts >> "$HOME"/logs/mirror_s3."$DATE".log

# SYS-436845/LOGMON-215
rm -f "$HOME"/s3_prod/*.err
find "$HOME"/s3_prod -type f -mtime +30 -delete

echo "parse S3"
#./parse.sh S3      |& ts >> "$HOME"/logs/parse_s3."$DATE".log
./parse_new.sh S3  |& ts >> "$HOME"/logs/parse_new_s3."$DATE".log

echo "parse GS"
#./parse.sh GS      |& ts >> "$HOME"/logs/parse_gs."$DATE".log
./parse_new.sh GS  |& ts >> "$HOME"/logs/parse_new_gs."$DATE".log

echo "parse OP"
#./parse.sh OP      |& ts >> "$HOME"/logs/parse_op."$DATE".log
./parse_new.sh OP  |& ts >> "$HOME"/logs/parse_new_op."$DATE".log


#dow=$(date +%u) # 1=Monday
dom=$(date +%e)
# Kurtis mirrors on 8th of month, pre-check before
if [ "$dom" = "2" ] || [ "$dom" -eq "6" ] ; then
    echo "bigquery_export"
    #./bigquery_objects.sh  |& ts >> "$HOME"/logs/bigquery_objects."$DATE".log
    #./bigquery_cloudian.sh  |& ts >> "$HOME"/logs/bigquery_cloudian."$DATE".log
    ./bigquery_export.sh  |& ts >> "$HOME"/logs/bigquery_export."$DATE".log
    ./bigquery_report.sh  |& ts >> "$HOME"/logs/bigquery_report."$DATE".log
    #./bigquery_summary.sh |& ts >> "$HOME"/logs/bigquery_summary."$DATE".log

    sed -re ' :restart ; s/([0-9])([0-9]{3})($|[^0-9])/\1,\2\3/ ; t restart ' \
        < "$HOME"/logs/bigquery_report."$DATE".log | \
        mailx -s "$HOST BigQuery Report" vartanianmh@ncbi.nlm.nih.gov
fi

echo "    --- Daily Processing Complete ($$) ---"
date
