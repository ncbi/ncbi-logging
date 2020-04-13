#!/bin/bash

cd /panfs/traces01.be-md.ncbi.nlm.nih.gov/strides-analytics/sra_prod || exit

for x in panfs*gz; do
    d=`echo "$x" | pcregrep -o "20[\d]{6}" | head -1`
    d="/panfs/traces01.be-md.ncbi.nlm.nih.gov/strides-analytics/sra_prod2/$d"
    mkdir -p "$d"
    cp -n -v "/panfs/traces01.be-md.ncbi.nlm.nih.gov/strides-analytics/sra_prod/$x" \
        "$d"
done
