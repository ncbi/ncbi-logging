#!/bin/bash
#find /panfs/traces01.be-md.ncbi.nlm.nih.gov/strides-analytics/sra_prod -size +4G -print0 | xargs -0 -n 1 -P 8 ~/strides/split_big.sh

cd /tmp/mike_logs/sra_prod || exit
file=$1
outfile=$(echo "$file" | tr -d '/')

echo "Working $file -> $outfile"
gunzip -c -d "$file" | split -d -l10000000 - "$outfile-split_"
#gzip -v $outfile-split_*
echo "Finished $file"
#rm -f "$file"
