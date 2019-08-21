#!/bin/bash

date
SSH_ENV="$HOME/.ssh/environment.$HOSTNAME"
# shellcheck source=/home/vartanianmh/strides/strides_env.sh
. "$SSH_ENV"

if ! lsof -ti:5433
then
    echo "Restarting tunnel"
    ssh -nvx \
    -L 5433:strides-analytics-db.cluster-cyen7j2us3rv.us-east-1.rds.amazonaws.com:5432 \
    vartanianmh@174.129.69.52 \
    'while true; do date; sleep 30; done'
fi
