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

psql -h localhost -d grafana -X << HERE
    CREATE index export_start on export (start_ts);
    CREATE index export_source on export (source);
    CLUSTER export using export_start;
    select count(*) from export;
    ANALYZE EXPORT;


    CREATE TEMP TABLE ips_export as select distinct ip, ip_int from export;

    DROP TABLE IF EXISTS ips_export2;
    CREATE TABLE ips_export2 as
        select ips_export.ip, ip_int, domain, city_name, country_code
        from ips_export, ip2location_db11_ipv4, rdns
        WHERE
            BOX(POINT(IP_FROM,IP_FROM),POINT(IP_TO,IP_TO)) @>
            BOX(POINT (IP_INT,IP_INT), POINT(IP_INT,IP_INT))
            AND RDNS.IP=ips_export.IP ;
    CREATE INDEX ips_export2_ip on ips_export2(ip);
    CREATE INDEX ips_export2_ipint on ips_export2(ip_int);


    DROP TABLE IF EXISTS export_joined;
    CREATE TABLE EXPORT_joined as
        SELECT STATUS, CMDS, BYTECOUNT,
            AGENT, CNT, ACC, START_TS, END_TS, SOURCE,
            HOST,
            CITY_NAME, COUNTRY_CODE, domain,
            export.ip, export.ip_int
        FROM EXPORT, IPS_EXPORT2
        WHERE
            export.ip_int=ips_export2.ip_int;
        CREATE index export_joined_start on export_joined (start_ts);
        CREATE index export_joined_source on export_joined (source);
    ANALYZE EXPORT_joined;

    ALTER TABLE cloud_sessions RENAME TO cloud_sessions_bak;
    ALTER TABLE export_joined RENAME TO cloud_sessions;
    DROP TABLE IF EXISTS cloud_sessions_bak;
HERE
