#!/bin/bash

# shellcheck source=/home/vartanianmh/strides/strides_env.sh
. "$HOME/strides/strides_env.sh"

cd "$PANFS/export" || exit

#psql -h localhost -d grafana -X << HERE
psql -h localhost -p 5433 -U postgres postgres << HERE
    DROP TABLE IF EXISTS export;
    CREATE TABLE export (
    status text,
    ip text,
    host text,
    cmds text,
    bytecount bigint,
    agent text,
    cnt bigint,
    acc text,
    start_ts timestamp,
    end_ts timestamp,
    source text,
    ip_int double precision
);

    DROP TABLE IF EXISTS export_objects;
    CREATE TABLE export_objects (data jsonb);
HERE

# export.20190822.000000000024.csv.gz
if [ ! -s export."$DATE".000000000025.csv.gz ]; then
    echo "No exports present, aborting"
    exit 1
fi

# NOTE: Not profitable to do this in parallel, some kind of contention
for x in export."$DATE".*.gz; do
    echo "Loading $x"
    psql -h localhost -p 5433 -U postgres postgres -c \
        "\\copy export FROM program 'gunzip -d -c $x' FORCE NOT NULL acc CSV HEADER;"
done

for x in "$PANFS/gs_prod/objects/$DATE."*gz "$PANFS/s3_prod/objects/$DATE."*gz  ; do
    echo "Loading $x"
    psql -h localhost -p 5433 -U postgres postgres -c \
        "\\copy export_objects FROM program 'gunzip -d -c $x';"
done

echo "Loaded exports"

cd "$HOME"/strides || exit

for sql in import.sql.*; do
    echo "Running $sql"
    #sqlformat -o tmp -k upper -i lower -r -a -s "$sql"
    #sed '/^$/d' < tmp > "$sql"
    psql -e -h localhost -p 5433 -U postgres postgres -f "$sql"
    echo
    echo "─────────────────────"
done

date
