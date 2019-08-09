#!/bin/bash
date
SSH_ENV="$HOME/.ssh/environment.$HOSTNAME"
. "$SSH_ENV"

if ! lsof -ti:5433
then
    echo "Restarting tunnel"
    ssh -nvx \
    -L 5433:strides-analytics-db.cluster-cyen7j2us3rv.us-east-1.rds.amazonaws.com:5432 \
    vartanianmh@174.129.69.52 \
    'sleep 3600' &
else
    echo "SSH tunnel already running"
fi
