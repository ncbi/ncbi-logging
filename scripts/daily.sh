#!/bin/bash

echo "    --- Daily Processing Beginning ($$) ---"

cd "$HOME/ncbi-logging/scripts" || exit

# shellcheck source=strides_env.sh
. ./strides_env.sh

set +e

mkdir -p "$HOME"/logs

/opt/panfs/bin/pan_df -H /panfs/traces01.be-md.ncbi.nlm.nih.gov/strides-analytics/

date; echo "mirror.sh GS"
./mirror.sh GS | ts >> "$HOME"/mirror_gs.log 2>&1

#date; echo "mirror.sh OP"
#./mirror.sh OP | ts >> "$HOME"/mirror_op.log 2>&1

date; echo "mirror.sh S3"
./mirror.sh S3 | ts >> "$HOME"/mirror_s3.log 2>&1

date; echo "mirror.sh Splunk"
./mirror.sh Splunk | ts >> "$HOME"/mirror_splunk.log 2>&1


date; echo "parse S3"
./parse.sh S3      | ts >> "$HOME"/parse_s3.log 2>&1
./parse_new.sh S3  | ts >> "$HOME"/logs/parse_new_s3."$DATE".log 2>&1

date; echo "parse GS"
./parse.sh GS      | ts >> "$HOME"/parse_gs.log 2>&1
./parse_new.sh GS  | ts >> "$HOME"/logs/parse_new_gs."$DATE".log 2>&1

#date; echo "parse OP"
#./parse.sh OP      | ts >> "$HOME"/parse_op.log 2>&1
#./parse_new.sh OP  | ts >> "$HOME"/logs/parse_new_op."$DATE".log 2>&1

date; echo "parse Splunk"
./parse.sh Splunk   | ts >> "$HOME"/parse_splunk.log 2>&1
./parse_new.sh Splunk   | ts >> "$HOME"/parse_new_splunk.log 2>&1

date; echo "s3_lister"
./s3_lister.sh | ts >> "$HOME"/s3_lister."$DATE".log 2>&1
./mirror_obj.sh | ts >> "$HOME"/mirror_obj."$DATE".log 2>&1

dow=$(date +%u)
if [ "$dow" = "1" ] || [ "$dow" = "4" ]; then
    date; echo "bigquery_export"
    ./bigquery_export.sh  | ts >> "$HOME"/logs/bigquery_export."$DATE".log 2>&1
    ./bigquery_report.sh  | ts >> "$HOME"/logs/bigquery_report."$DATE".log 2>&1
    ./bigquery_summary.sh | ts >> "$HOME"/logs/bigquery_summary."$DATE".log 2>&1

    mailx -s "BigQuery Report" vartanianmh@ncbi.nlm.nih.gov < "$HOME"/logs/bigquery_report."$DATE".log
fi

echo "    --- Daily Processing Complete ($$) ---"
date
