#!/bin/bash

# shellcheck source=/home/vartanianmh/strides/strides_env.sh
. "$HOME/strides/strides_env.sh"

SRCDIR="$PWD"

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
listen_addresses = '*'
password_encryption = 'scram-sha-256'
max_connections = 100
shared_buffers = 32GB
effective_cache_size = 100GB # intprod11 usually around this
work_mem = 2GB
max_worker_processes = 48
max_parallel_workers_per_gather = 16
max_parallel_workers = 16
log_autovacuum_min_duration = 250ms
checkpoint_completion_target = 0.9
default_statistics_target = 500
max_wal_size = 4GB
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

# Into pg_hba.conf:
# host    strides_analytics all             130.14.24.69/32 scram-sha-256
# host    grafana vartanianmh samehost trust

pg_ctl -l "$HOME/$PGVER/logs/logfile" start
createdb -h /tmp/ grafana
