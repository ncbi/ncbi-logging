#!/usr/bin/bash

set -o nounset # same as -u
#set -o errexit # same as -e
set -o pipefail
shopt -s nullglob globstar

export PGVER="postgresql-11.5"
export GRAFANAVER="grafana-6.3.5" # Update conf/grafana.ini too
export PORT=3389
export PGDATA="$HOME/pgdata"
export PGHOST="localhost"
export PGDATABASE="grafana"
export PGHOST="/tmp/"
export PATH="$HOME/$PGVER/bin:~/.local/bin:/opt/python-all/bin:$PATH"
export GUNIHOST=intprod11
export GUNIPORT=20817
export AWS_PROFILE="strides-analytics"
export GF_PLUGIN_DIR="$HOME/$GRAFANAVER/plugins"
export LD_LIBRARY_PATH="$HOME/$PGVER/lib"
export LD_LIBRARY_PATH="$HOME/lib:$LD_LIBRARY_PATH"
export LD_LIBRARY_PATH="$HOME/$GRAFANAVER/lib:$LD_LIBRARY_PATH"
export LOGDIR=/tmp/mike_logs
export PANFS="/panfs/traces01.be-md.ncbi.nlm.nih.gov/strides-analytics"
DATE=$(date "+%Y%m%d") #_%H%M%S
export DATE
export "RAMDISK=/dev/shm"
YESTERDAY_DASH=$(date -d "yesterday" "+%Y-%m-%d")
export YESTERDAY_DASH
YESTERDAY=${YESTERDAY_DASH//-}
export YESTERDAY


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
