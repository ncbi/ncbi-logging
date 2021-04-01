#!/bin/bash

echo "    --- Daily Processing Beginning ($$) ---"

cd "$HOME/ncbi-logging/scripts" || exit

# shellcheck source=strides_env.sh
. ./strides_env.sh

set +e

mkdir -p "$HOME"/logs

/opt/panfs/bin/pan_df -H /panfs/traces01.be-md.ncbi.nlm.nih.gov/strides-analytics/

homespace=$( df  "$HOME" | tail  -1 | tr -s ' ' | cut -d ' ' -f 4)
if [ "$homespace" -lt 500000 ]; then
    df -HT "$HOME" | mailx -s "$HOME low on space" vartanianmh@ncbi.nlm.nih.gov
fi

#echo "mirror.sh GS"
#./mirror.sh GS |& ts >> "$HOME"/logs/mirror_gs."$DATE".log

#echo "mirror.sh OP"
#./mirror.sh OP |& ts >> "$HOME"/logs/mirror_op."$DATE".log

echo "mirror.sh S3"
./mirror.sh S3 |& ts >> "$HOME"/logs/mirror_s3."$DATE".log

echo "parse S3"
./parse.sh S3      |& ts >> "$HOME"/logs/parse_s3."$DATE".log
./parse_new.sh S3  |& ts >> "$HOME"/logs/parse_new_s3."$DATE".log

#echo "parse GS"
#./parse.sh GS      |& ts >> "$HOME"/logs/parse_gs."$DATE".log
#./parse_new.sh GS  |& ts >> "$HOME"/logs/parse_new_gs."$DATE".log

#echo "parse OP"
#./parse.sh OP      |& ts >> "$HOME"/logs/parse_op."$DATE".log
#./parse_new.sh OP  |& ts >> "$HOME"/logs/parse_new_op."$DATE".log

echo "    --- Daily Processing Complete ($$) ---"
date
