#!/bin/bash

# shellcheck source=/home/vartanianmh/strides/strides_env.sh
. "$HOME/strides/strides_env.sh"

SRCDIR="$PWD"
#DATE=$(date "+%Y%m%d") #_%H%M%S

cd "$HOME" || exit
rm -rf "$PGVER"
tar -xaf "$SRCDIR/$PGVER.rel.tar.gz"
cd "$HOME/$PGVER" || exit

mkdir -p "logs"
rm -rf "$PGDATA"
mkdir -p "$PGDATA"

initdb --data-checksums
cd "$HOME" || exit
#patch -p0 -i "$SRCDIR/postgresql.conf.patch" # Produce with diff -u
cat <<-EOF > "$PGDATA/postgresql.conf"
port = 5432
max_connections = 100
shared_buffers = 16GB # Entire DB is ~16GB
effective_cache_size = 100GB # intprod11 usually around this
work_mem = 1GB
max_worker_processes = 16
max_parallel_workers_per_gather = 8
max_parallel_workers = 16
log_autovacuum_min_duration = 250ms
checkpoint_completion_target = 0.9
default_statistics_target = 500
max_wal_size = 1GB
min_wal_size = 512MB
synchronous_commit = off
datestyle = 'iso, mdy'
timezone = 'US/Eastern'
lc_messages = 'en_US.UTF-8'
lc_monetary = 'en_US.UTF-8'
lc_numeric = 'en_US.UTF-8'
lc_time = 'en_US.UTF-8'
default_text_search_config = 'pg_catalog.english'
EOF

pg_ctl -l "$HOME/$PGVER/logs/logfile" start
createdb -h /tmp/ grafana
