#!/bin/bash

cd /tmp/mike_logs/ || exit

prev=""
d=$1
#for d in S3-sra-pub-run-[234567]-logs; do
    cd "/tmp/mike_logs/$d" || exit
    # 2019-05-02-17-25-16-27D12109D24112AE
    for f in *; do
        d=${f:0:10}
        d=${d//-}
        d="/tmp/mike_logs/s3_prod/$d"
        if [ "$prev" != "$d" ]; then
            mkdir -p "$d"
            prev="$d"
        fi
    #    if [ ! -e "$d/$f" ]; then
            cp -n -v "$f" "$d"
    #    fi
    done
