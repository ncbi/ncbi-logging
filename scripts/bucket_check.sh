#!/usr/bin/bash

export PATH="$HOME/bin:$HOME/.local/bin:/opt/python-all/bin:$HOME/google-cloud-sdk/bin:/netopt/ncbi_tools64/bin/:$PATH"

umask u=rwx,g=,o=

set -u

date

cd /home/vartanianmh || exit

DATE=$(date "+%Y%m%d") #_%H%M%S
YESTERDAY_DASH=$(date -d "yesterday" "+%Y-%m-%d")
export YESTERDAY=${YESTERDAY_DASH//-/}
export TMP="/tmp/$USER"
if [ ! -d "$TMP" ]; then
  mkdir -p "$TMP"
fi

OUTFILE="$TMP/bucket_check.$DATE.out"

"$HOME/ncbi-logging/scripts/bucket_check.py" >"$OUTFILE" 2>>"$HOME/logs/bucket_check.log"

if [ -s "$OUTFILE" ]; then
        mailx -s "Strides Analytics Bucket Check" vartanianmh@ncbi.nlm.nih.gov < "$OUTFILE"
fi

exit 0
