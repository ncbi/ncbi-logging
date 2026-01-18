#!/bin/bash

cd "$HOME/ncbi-logging/scripts" || exit

# shellcheck source=strides_env.sh
. ./strides_env.sh

echo "    --- Daily Processing for $DATE Beginning ($$) ---"

set +e

mkdir -p "$HOME"/logs
find "$HOME/logs" ! -name "*xz" -mtime +1 -size +2M -exec xz -9 {} \;
find "$HOME/done" -mtime +60 -delete

#panspace=$(/opt/panfs/bin/pan_df -H /panfs/traces01.be-md.ncbi.nlm.nih.gov/strides-analytics/ | tail -1 | tr -s ' ' | cut -d ' ' -f 5 | tr -d '%')
#if [ "$panspace" -gt 95 ]; then
#    /opt/panfs/bin/pan_df -H /panfs/traces01.be-md.ncbi.nlm.nih.gov/strides-analytics/ | mailx -s "panfs low on space" vartanianmh@ncbi.nlm.nih.gov
#fi

vastspace=$(df -H "$VASTFS" | tail -1 | tr -s ' ' | cut -d' ' -f 5 | tr -d '%')
if [ "$vastspace" -gt 95 ]; then
    df -H "$VASTFS" | mailx -s "VAST fs low on space" vartanianmh@ncbi.nlm.nih.gov
fi

homespace=$(df "$HOME" | tail -1 | tr -s ' ' | cut -d ' ' -f 4)
if [ "$homespace" -lt 500000 ]; then
    df -HT "$HOME" | mailx -s "$HOME low on space" vartanianmh@ncbi.nlm.nih.gov
fi

echo "mirror.sh OP"
./mirror.sh OP |& ts >> "$HOME"/logs/mirror_op."$DATE".log

echo "s3_lister"
./s3_lister.sh |& ts >> "$HOME"/logs/s3_lister."$DATE".log
./mirror_obj.sh |& ts >> "$HOME"/logs/mirror_obj."$DATE".log

echo "mirror.sh GS"
./mirror.sh GS |& ts >> "$HOME"/logs/mirror_gs."$DATE".log

echo "sra_prod.sh"
./sra_prod.sh |& ts >>  "$HOME"/logs/sra_prod."$DATE".log

echo "mirror.sh S3"
./mirror.sh S3 |& ts >> "$HOME"/logs/mirror_s3."$DATE".log

# SYS-436845/LOGMON-215
#rm -f /panfs/traces01.be-md.ncbi.nlm.nih.gov/strides-analytics/s3_prod/*.err
#find /panfs/traces01.be-md.ncbi.nlm.nih.gov/strides-analytics/s3_prod -type f -mtime +30 -delete

rm -f "$VASTFS"/s3_prod/*.err
find "$VASTFS"/s3_prod -type f -mtime +30 -delete

#echo "mirror.sh Splunk"
#./mirror.sh Splunk |& ts >> "$HOME"/logs/mirror_splunk."$DATE".log &

echo "SRA Main"
./sra_main.sh |& ts >> "$HOME"/logs/sra_main."$DATE".log

echo "parse S3"
./parse.sh S3 |& ts >> "$HOME"/logs/parse_s3."$DATE".log &
./parse_new.sh S3 |& ts >> "$HOME"/logs/parse_new_s3."$DATE".log &

echo "parse GS"
./parse.sh GS |& ts >> "$HOME"/logs/parse_gs."$DATE".log
./parse_new.sh GS |& ts >> "$HOME"/logs/parse_new_gs."$DATE".log

echo "parse OP"
./parse.sh OP |& ts >> "$HOME"/logs/parse_op."$DATE".log
./parse_new.sh OP |& ts >> "$HOME"/logs/parse_new_op."$DATE".log

echo "waiting..."
date
jobs
wait
date

#echo "parse Splunk"
#./parse.sh Splunk   |& ts >> "$HOME"/logs/parse_splunk."$DATE".log
#./parse_new.sh Splunk   |& ts >> "$HOME"/logs/parse_new_splunk."$DATE".log

#dow=$(date +%u) # 1=Monday
dom=$(date +%e)
month=$(date +%m)

# Annual refresh on 1/2
if [ "$month" -eq 1 ]; then
    if [ "$dom" -eq 2 ]; then
        ./bigquery_annual.sh annual |& ts >> "$HOME"/logs/bigquery_annual_refresh."$DATE".log
    fi
fi

# Kurtis mirrors on 8th of month, pre-check before
if [ "$dom" -eq 2 ] || [ "$dom" -eq 5 ] || [ "$dom" -eq 18 ]; then
    echo "bigqueries"
    # ./bigquery_objects.sh |& ts >> "$HOME"/logs/bigquery_objects."$DATE".log
    ./bigquery_cloudian.sh |& ts >> "$HOME"/logs/bigquery_cloudian."$DATE".log
    #./bigquery_export.sh  |& ts >> "$HOME"/logs/bigquery_export."$DATE".log
    ./bigquery_annual_v3.sh |& ts >> "$HOME"/logs/bigquery_annual."$DATE".log
    ./bigquery_report.sh |& ts >> "$HOME"/logs/bigquery_report."$DATE".log
    ./bigquery_summary.sh |& ts >> "$HOME"/logs/bigquery_summary."$DATE".log

    # cat bigquery_report."$DATE".log | perl -pe 's/\d{1,3}(?=(\d{3})+(?!\d))/$&,/g'
    mailx -s "BigQuery Report" vartanianmh@ncbi.nlm.nih.gov < "$HOME"/logs/bigquery_report."$DATE".log
fi

DONEFILE="${HOME}/done/daily_${YESTERDAY}.done"
if [ ! -e "$DONEFILE" ]; then
    echo "No done"
    echo "No done" | mailx -s "$HOST daily not done $YESTERDAY" vartanianmh@ncbi.nlm.nih.gov
fi
DONEFILE="${HOME}/done/daily_${TODAY}.done"
touch "$DONEFILE"

echo "    --- Daily Processing for $DATE Complete ($$) ---"
date
