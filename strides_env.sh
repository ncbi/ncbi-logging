#!/usr/bin/bash

set -o nounset # same as -u
#set -o errexit # same as -e
set -o pipefail
shopt -s nullglob globstar

export PGVER="postgresql-11.5"
export GRAFANAVER="grafana-6.4.1" # Update conf/grafana.ini too
export PORT=3389
export PGDATA="$HOME/pgdata"
export PGHOST="localhost"
export PGDATABASE="grafana"
export PGHOST="/tmp/"
export PATH="$HOME/$PGVER/bin:$HOME/.local/bin:/opt/python-all/bin:$HOME/google-cloud-sdk/bin:$PATH"
export GUNIHOST=intprod11
export GUNIPORT=20817
export AWS_PROFILE="strides-analytics"
export GF_PLUGIN_DIR="$HOME/$GRAFANAVER/plugins"
export LD_LIBRARY_PATH="$HOME/$PGVER/lib"
export LD_LIBRARY_PATH="$HOME/lib:$HOME/.local/lib:$LD_LIBRARY_PATH"
export LD_LIBRARY_PATH="$HOME/$GRAFANAVER/lib:$LD_LIBRARY_PATH"
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

cd /home/vartanianmh/strides || exit

case "$HOSTNAME" in
    iebdev11)
        renice +19 -p $$ > /dev/null 2>&1
        ;;
    lmem05)
        #renice +19 -p $$ > /dev/null 2>&1
        ;;
    *)
        ;;
esac

echo "____________________________"
date
