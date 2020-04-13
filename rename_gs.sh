#!/bin/bash

cd $LOGDIR/ || exit

for d in sra-pub-run-*-logs; do
    cd "$LOGDIR/$d" || exit
    for f in *usage*; do
    # sra-pub-run-1_usage_2019_06_24_03_00_00_07378a0db72e87e0b4_v0
        d=`echo "$f" | tr -d '_' | cut -c19-26`
        d="$LOGDIR/gs_prod/$d"
        mkdir -p "$d"
        cp -n -v "$f" "$d"
    done
done

