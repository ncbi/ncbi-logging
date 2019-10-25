#!/bin/bash

# shellcheck source=/home/vartanianmh/strides/strides_env.sh
. "$HOME/strides/strides_env.sh"

pip3.7 list --outdated

psql -h localhost -d grafana -t -X -c 'select ips.ip  from ips left join rdns on ips.ip=rdns.ip where rdns.ip is null' > ips.new

# insert into rdns (ip, domain) select ips.ip, 'unknown' from ips left join rdns on ips.ip=rdns.ip where rdns.ip is null
#./cloud_ips.py | sort -k1,1 -u > cloud_ips.tsv 2>/dev/null
#psql -h localhost -d grafana -c "delete from cloud_ips"
#psql -h localhost -d grafana -c "\\copy cloud_ips FROM cloud_ips.tsv WITH DELIMITER E'\t'"

tr -d ' ' < ips.new | sort -u | sort -R | ./ip_lookup.py > ips.new.tsv 2>/dev/null
cat ips.new.tsv cloud_ips.tsv | sort -k1,1 -u > ips.tsv
cp ips.tsv "ips.tsv.$DATE"
psql -h localhost -d grafana -c "\\copy rdns FROM ips.new.tsv WITH DELIMITER E'\t'"

psql -h localhost -d grafana -f rdns.sql > /dev/null 2>&1
psql -d strides_analytics -U sa_prod_write -f rdns.sql > /dev/null 2>&1
psql -h localhost -d grafana -f accs.sql > /dev/null 2>&1

psql << USAGE
    SELECT nspname || '.' || relname AS "relation",
        pg_size_pretty(pg_total_relation_size(C.oid)) AS "total_size"
    FROM pg_class C
    LEFT JOIN pg_namespace N ON (N.oid = C.relnamespace)
    WHERE nspname NOT IN ('pg_catalog', 'information_schema')
        AND C.relkind <> 'i'
        AND nspname !~ '^pg_toast'
    ORDER BY pg_total_relation_size(C.oid) DESC
    LIMIT 20;

    select source,
    count(*) as recs, sum(cnt) as sumcnt, count(distinct ip) as distips
    from cloud_sessions
    group by source;
USAGE

./import.sh >> ~/import.log 2>&1
./import_write.sh >> ~/import_write.log 2>&1

pg_dump -h localhost -d grafana | \
     xz -T 20 -c > "$PANFS/pg_dumps/pg_dump.$DATE.xz" &

pg_dump -U sa_prod_write -d strides_analytics | \
     xz -T 20 -c > "$PANFS/pg_dumps/pg_dump_sa.$DATE.xz" &

#e4defrag -v ~/pgdata/ >> ~/defrag.log 2>&1 &

date

