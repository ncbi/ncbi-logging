#!/bin/bash

# shellcheck source=/home/vartanianmh/strides/strides_env.sh
. "$HOME/strides/strides_env.sh"

cd "$PANFS/export" || exit

psql -h localhost -d grafana -X << HERE
    DROP TABLE IF EXISTS export;
    CREATE TABLE export (
    status text,
    ip text,
    host text,
    cmds text,
    bytecount bigint,
    agent text,
    cnt int,
    acc text,
    start_ts timestamp,
    end_ts timestamp,
    source text,
    ip_int double precision
);
HERE

# NOTE: Not profitable to do this in parallel, some kind of contention
for x in export."$DATE".*.gz; do
    echo "Loading $x"
    psql -h localhost -d grafana -c \
        "\\copy export FROM program 'gunzip -d -c $x' FORCE NOT NULL acc CSV HEADER;"
done

echo "Loaded export"

time psql -h localhost -d grafana << HERE
/*
    CREATE index export_start on export (start_ts);
    CREATE index export_source on export (source);
    CLUSTER export using export_start;
*/
select count(*) AS export_count FROM export;


CREATE TEMP TABLE ips_export AS select distinct ip, ip_int FROM export;

DROP TABLE IF EXISTS ips_export2;
CREATE TABLE ips_export2 as
    select ips_export.ip, ip_int, domain, city_name, country_code
    FROM ips_export, ip2location_db11_ipv4, rdns
    WHERE
        BOX(POINT(ip_FROM,ip_FROM),POINT(ip_to,ip_to)) @>
        BOX(POINT (ip_int,ip_int), POINT(ip_int,ip_int))
        AND rdns.ip=ips_export.ip ;

SELECT count(*) AS ips_export2_clount FROM ips_export2;

CREATE INDEX ips_export2_ip on ips_export2(ip);
CREATE INDEX ips_export2_ipint on ips_export2(ip_int);


DROP TABLE IF EXISTS export_joined;
CREATE TABLE export_joined as
    SELECT status, cmds, bytecount,
        agent, cnt, acc, start_ts, end_ts, source,
        host,
        city_name, country_code, domain,
        export.ip, export.ip_int
    FROM export, ips_export2
    WHERE
        export.ip_int=ips_export2.ip_int
    ORDER BY source, start_ts;

SELECT count(*) AS export_joined_count FROM EXPORT_JOINED;

BEGIN;
    ALTER TABLE cloud_sessions RENAME TO cloud_sessions_bak;
    ALTER TABLE export_joined RENAME TO cloud_sessions;
COMMIT;

SELECT count(*) AS cloud_sessions_count FROM cloud_sessions;

    DROP index cloud_sessions_start;
    DROP index cloud_sessions_source;
    CREATE index cloud_sessions_start on cloud_sessions (start_ts);
    CREATE index cloud_sessions_source on cloud_sessions (source);

-- Materialized views, AS tables in case we need to index them
BEGIN;
    DROP TABLE IF EXISTS last_used;
    CREATE TABLE last_used as
        select acc, max(last) AS last
        FROM  (
            select acc, date_trunc('day', start_ts) AS last
            FROM cloud_sessions
            where source='SRA'
            and acc ~ '[DES]RR[\d\.]{6,10}'
            union all
            select run AS acc, '2018-12-01 00:00:00'::timestamp AS last
            FROM public
        ) AS roll2
        group by acc;
COMMIT;

BEGIN;
    DROP TABLE IF EXISTS last_used_interval;
    CREATE TABLE last_used_interval as
    select case
        when date_trunc('day', age(localtimestamp, last)) < interval '30 days'
        then 'downloaded in last 30 days'
        when date_trunc('day', age(localtimestamp, last)) > interval '180 days'
        then 'never downloaded'
        else 'downloaded 30..180 days ago'
    end AS metric,
    count(*) AS value
    FROM last_used
    group by metric;
COMMIT;

BEGIN;
    DROP TABLE IF EXISTS downloads_by_ip;
    CREATE TABLE downloads_by_ip as
        SELECT date_trunc('day', start_ts) AS time,
        source,
        count(distinct ip) AS unique_users
        FROM
        cloud_sessions
        WHERE
        ( cmds like '%GET%' or cmds like '%HEAD%' )
        AND source != 'SRA'
        GROUP BY time, source
        ORDER BY time;
COMMIT;

BEGIN;
    DROP TABLE IF EXISTS last_used_interval;
    CREATE TABLE last_used_interval as
    select case
        when date_trunc('day', age(localtimestamp, last)) < interval '30 days'
        then 'downloaded in last 30 days'
        when date_trunc('day', age(localtimestamp, last)) > interval '180 days'
        then 'never downloaded'
        else 'downloaded 30..180 days ago'
    end AS metric,
    count(*) AS value
    FROM last_used
    group by metric;
COMMIT;

BEGIN;
    DROP TABLE IF EXISTS daily_downloads;
    CREATE TABLE daily_downloads as
    SELECT
        date_trunc('day', start_ts) AS time,
        source,
        sum(bytecount) AS bytes,
        sum(cnt) AS downloads
    FROM
    cloud_sessions
    WHERE
    ( cmds like '%GET%' or cmds like '%HEAD%' )
    GROUP BY "time", source
    ORDER BY "time";
COMMIT;

BEGIN;
    DROP TABLE IF EXISTS cloud_downloads;
    CREATE TABLE cloud_downloads AS
    select source || ' -> ' || domain
    || ' @ ' || city_name
    || ', ' || country_code
    AS Path,
    sum(cnt) AS Downloads, sum(bytecount) AS Bytes,
    date_trunc('day', start_ts) AS time
    FROM cloud_sessions
    where
    ( cmds like '%GET%' or cmds like '%HEAD%' )
    AND source!='SRA'
    group by time, path
    order by Downloads desc;
COMMIT;

BEGIN;
    DROP TABLE IF EXISTS download_domains;
    CREATE TABLE download_domains as
        select domain,
        date_trunc('day', start_ts) AS time,
        sum(bytes) AS Bytes
        FROM sra_cloud
        group by domain, time;
COMMIT;

BEGIN;
    DROP TABLE IF EXISTS cloud_organisms;
    CREATE TABLE cloud_organisms as
    select initcap(acc_desc) AS Organism,
    count(*) AS popularity,
    date_trunc('day', start_ts) AS time
    FROM cloud_sessions, accs
    where
    ( cmds like '%GET%' or cmds like '%HEAD%' )
    AND cloud_sessions.acc=accs.acc
    group by time, acc_desc
    order by popularity desc
    limit 500;
COMMIT;

BEGIN;
    DROP TABLE IF EXISTS acc_last_acc;
    CREATE TABLE acc_last_acc as
    select last AS time, count(*) AS accessions FROM (
    select max(last) AS last
    FROM sra_last_used
    group by acc) AS roll
    group by last
    order by last;
COMMIT;

BEGIN;
    DROP TABLE IF EXISTS sra_cloud_downloads;
    CREATE TABLE sra_cloud_downloads as
    SELECT
        date_trunc('day', start_ts) AS time,
        sum(bytes) AS bytes,
        sum(cnt) AS downloads
    FROM
        sra_cloud
    GROUP BY "time";
COMMIT;


DROP TABLE IF EXISTS cloud_sessions_bak;
DROP TABLE IF EXISTS export;
DROP TABLE IF EXISTS ips_export2;
HERE

date
