#!/usr/bin/bash

set -o nounset # same as -u
set -o errexit # same as -e
set -o pipefail
shopt -s nullglob globstar

export PGVER="postgresql-12.3"
export PGDATA="$HOME/pgdata12"
export PATH="$HOME/$PGVER/bin:$HOME/.local/bin:/opt/python-all/bin:$HOME/google-cloud-sdk/bin:$PATH"
export AWS_PROFILE="strides-analytics"
export LD_LIBRARY_PATH="$HOME/$PGVER/lib"
export LD_LIBRARY_PATH="$HOME/lib:$HOME/.local/lib:$LD_LIBRARY_PATH"
export LOGDIR=$HOME
export PANFS="/panfs/traces01.be-md.ncbi.nlm.nih.gov/strides-analytics"
DATE=$(date "+%Y%m%d") #_%H%M%S
export DATE
export "RAMDISK=/dev/shm"
YESTERDAY_DASH=$(date -d "yesterday" "+%Y-%m-%d")
export YESTERDAY_DASH
YESTERDAY=${YESTERDAY_DASH//-}
export YESTERDAY
YESTERDAY_UNDER=$(date -d "yesterday" "+%Y_%m_%d")
export YESTERDAY_UNDER

SQLCACHE="$RAMDISK/${USER}_${DATE}"
mkdir -p "$SQLCACHE"
if [ ! -e "$SQLCACHE/buckets.db" ]; then
    gsutil cp gs://strides_analytics_cfg/buckets.db "$SQLCACHE/buckets.db"
fi

# Usage: results=$(sqlcmd "select foo from bar")
sqlcmd () {
    local query="$1"
    sqlite3 -batch "${SQLCACHE}/buckets.db" << EOF
.headers off
.mode tabs
.nullvalue NULL
${query}
EOF
}

echo "____________________________"
date
