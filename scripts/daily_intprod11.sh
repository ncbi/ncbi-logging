#!/bin/bash

echo "    --- Daily Processing Beginning ($$) ---"

cd "$HOME/ncbi-logging/scripts" || exit

# shellcheck source=strides_env.sh
. ./strides_env.sh

set +e

mkdir -p "$HOME"/logs

#/opt/panfs/bin/pan_df -H /panfs/traces01.be-md.ncbi.nlm.nih.gov/strides-analytics/

homespace=$( df  "$HOME" | tail  -1 | tr -s ' ' | cut -d ' ' -f 4)
if [ "$homespace" -lt 500000 ]; then
    df -HT "$HOME" | mailx -s "$HOME low on space" vartanianmh@ncbi.nlm.nih.gov
fi

echo "mirror.sh GS"
./mirror.sh GS |& ts >> "$HOME"/logs/mirror_gs."$DATE".log

echo "mirror.sh OP"
./mirror.sh OP |& ts >> "$HOME"/logs/mirror_op."$DATE".log

echo "mirror.sh S3"
./mirror.sh S3 |& ts >> "$HOME"/logs/mirror_s3."$DATE".log

echo "parse S3"
#./parse.sh S3      |& ts >> "$HOME"/logs/parse_s3."$DATE".log
./parse_new.sh S3  |& ts >> "$HOME"/logs/parse_new_s3."$DATE".log

echo "parse GS"
#./parse.sh GS      |& ts >> "$HOME"/logs/parse_gs."$DATE".log
./parse_new.sh GS  |& ts >> "$HOME"/logs/parse_new_gs."$DATE".log

echo "parse OP"
#./parse.sh OP      |& ts >> "$HOME"/logs/parse_op."$DATE".log
./parse_new.sh OP  |& ts >> "$HOME"/logs/parse_new_op."$DATE".log


dow=$(date +%u)
if [ "$dow" = "1" ];  then
#if [ "$dow" = "1" ] || [ "$dow" = "4" ]; then # Mondays and Thursday
    echo "bigquery_export"
    #./bigquery_objects.sh  |& ts >> "$HOME"/logs/bigquery_objects."$DATE".log
    #./bigquery_cloudian.sh  |& ts >> "$HOME"/logs/bigquery_cloudian."$DATE".log
    ./bigquery_export.sh  |& ts >> "$HOME"/logs/bigquery_export."$DATE".log
    ./bigquery_report.sh  |& ts >> "$HOME"/logs/bigquery_report."$DATE".log
    #./bigquery_summary.sh |& ts >> "$HOME"/logs/bigquery_summary."$DATE".log

    mailx -s "$HOST BigQuery Report" vartanianmh@ncbi.nlm.nih.gov < "$HOME"/logs/bigquery_report."$DATE".log
fi

echo "    --- Daily Processing Complete ($$) ---"
date
