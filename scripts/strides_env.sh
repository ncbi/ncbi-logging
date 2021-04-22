#!/usr/bin/bash

set -o nounset # same as -u
set -o errexit # same as -e
set -o pipefail
shopt -s nullglob globstar

export SCRIPTDIR="$PWD"
export PREFIX="strides_analytics"
export PGVER="postgresql-12.3"
export PGDATA="$HOME/pgdata12"
export PATH="$HOME/$PGVER/bin:$HOME/.local/bin:/opt/python-all/bin:$HOME/google-cloud-sdk/bin:/netopt/ncbi_tools64/bin/:$PATH"
export PATH="$PATH:$HOME/devel/ncbi-logging/parser/bin:$HOME/ncbi-logging/parser/bin"
export AWS_PROFILE="strides-analytics"
export LD_LIBRARY_PATH="$HOME/$PGVER/lib"
export LD_LIBRARY_PATH="$HOME/lib:$HOME/.local/lib:$LD_LIBRARY_PATH"
export LOGDIR=$HOME
export TMP="/tmp/$USER"
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

HOST=$(hostname)
export HOST

indent() { sed 's/^/  /'; }

# GCP VM shouldn't have any other competing load, helps on iebdev
renice +19 -p $$ &> /dev/null

SQLCACHE="$RAMDISK/${USER}_${DATE}"
#SQLCACHE="$RAMDISK/${USER}"
mkdir -p "$SQLCACHE"
if [ ! -s "$SQLCACHE/buckets.db" ]; then
    echo "Refreshing sqlite cache"
    export CLOUDSDK_CORE_PROJECT="ncbi-logmon"
    gcloud config set account 253716305623-compute@developer.gserviceaccount.com

    gsutil cp gs://logmon_cfg/buckets.db "$SQLCACHE/buckets.db"
fi

# Usage: results=$(sqlcmd "select foo from bar")
sqlcmd () {
    local query="$1"
    sqlite3 -batch "${SQLCACHE}/buckets.db" << EOF
.headers off
.mode tabs
.nullvalue NULL
${query} ;
EOF
}

export STRIDES_SCOPE="public"

case "$HOSTNAME" in
    iebdev11)
        export STRIDES_SCOPE="public"
        renice +19 -p $$ > /dev/null 2>&1
        ;;
    intprod11)
        export STRIDES_SCOPE="private"
        #renice +19 -p $$ > /dev/null 2>&1
        ;;
    lmem14)
        export STRIDES_SCOPE="public"
        #renice +19 -p $$ > /dev/null 2>&1
        ;;
    *)
        ;;
esac

if [ "$STRIDES_SCOPE" == "private" ]; then
    export DATASET="strides_analytics_private"
    export MIRROR_BUCKET="gs://logmon_logs_private"
    export PARSE_BUCKET="gs://logmon_logs_parsed_private"
    export PARSE_VER="/v3"
else
    export DATASET="strides_analytics"
    export MIRROR_BUCKET="gs://logmon_logs"
    export PARSE_BUCKET="gs://logmon_logs_parsed_us"
    export PARSE_VER=""
fi

echo "____________________________"
date
