#!/bin/bash

# shellcheck source=/home/vartanianmh/strides/strides_env.sh
. "$HOME/strides/strides_env.sh"

DB=nginx_agent_sess.db

if [ ! -f $DB ] ; then
    touch $DB
fi

date
#for file in `ls -1 -R /panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive/*/*/*/srafiles*/access.log_????????.gz`;
for file in `ls -1 -R /tmp/vartanianmh/panfs*access.log_????????.gz`;
do
    grep "$file" "$DB"
    if [ $? -eq 0 ]; then
        echo "Already loaded $file"
    else
        echo "Processing $file"
        gunzip -d -c "$file" | tail -n 1000000 | ~/strides/nginx_agent_sess.py
        #gunzip -d -c $file | ~/strides/nginx_agent_sess.py
        echo "Processed  $file"
        echo "$file" >> "$DB"
    fi
done

date
