DROP TABLE IF EXISTS last_used_cold_2;
CREATE TABLE last_used_cold_2 as
    SELECT acc, max(last_used) AS last_used
    FROM  (
        SELECT acc, date_trunc('day', start_ts) AS last_used
        FROM cloud_sessions
        where
        ( cmds like '%GET%' or cmds like '%HEAD%' ) and
        domain not like '%nih.gov%'
        AND domain not like '%IANA Reserved%'
        acc ~ '^[DES]RR[\d\.]{6,10}'
        and source='NCBI'
        union all
        SELECT run AS acc, '2018-06-01 00:00:00'::timestamp AS last_used
        FROM public
    ) AS roll2
    group by acc
    order by acc;

GRANT SELECT ON TABLE last_used_cold_2 TO PUBLIC;

DROP TABLE IF EXISTS cold_2;
CREATE TABLE cold_2 AS
SELECT acc, max(last_used) as last_used, scientificname as organism, size_mb
from last_used_cold_2, public
where acc=run
group by acc, organism, size_mb;

GRANT SELECT ON TABLE cold_2 TO PUBLIC;

select date_trunc('week',last_used) as week, sum(size_mb::bigint) as size_mb
from cold_2
group by week
order by week desc;

select acc, scientificname, count(*), count(distinct ip)
from cloud_sessions, public
where acc in (select acc from dbgaps)
and  ( cmds like '%GET%' or cmds like '%HEAD%' )

        and domain not like '%nih.gov%'
        AND domain not like '%IANA Reserved%'
        and cloud_sessions.acc=public.run
group by acc, scientificname
order by count(*) desc limit 20

create temp table public_ts as
select run as acc,
        to_timestamp(releasedate, 'YYYY-MM-DD HH24:MI:SS') as releasedate,
        to_timestamp(loaddate, 'YYYY-MM-DD HH24:MI:SS') as loaddate
        from public;


create temp table first_public as
    select cloud_sessions.acc, min(start_ts - releasedate) as delay
    from cloud_sessions, public_ts
    where
    cloud_sessions.acc=public_ts.acc and
    ( cmds like '%GET%' or cmds like '%HEAD%' ) and
    domain not like '%nih.gov%' and
    domain not like '%IANA Reserved%'
    and releasedate > '2018-09-01'
    and cloud_sessions.acc not in (select acc from dbgaps)
    group by cloud_sessions.acc;

create temp table first_cloud as
    select cloud_sessions.acc, min(start_ts - releasedate) as delay
    from cloud_sessions, public_ts
    where
    cloud_sessions.acc=public_ts.acc and
    ( cmds like '%GET%' or cmds like '%HEAD%' ) and
    domain not like '%nih.gov%' and
    domain not like '%IANA Reserved%' and
    (domain like '%(AWS %)' or domain like '%(GCP%)')
    and releasedate > '2018-09-01'
    and cloud_sessions.acc not in (select acc from dbgaps)
    group by cloud_sessions.acc;

create temp table first_dbgap as
    select cloud_sessions.acc, min(start_ts -releasedate) as delay
    from cloud_sessions, public_ts
    where
    cloud_sessions.acc=public_ts.acc and
    ( cmds like '%GET%' or cmds like '%HEAD%' ) and
    domain not like '%nih.gov%' and
    domain not like '%IANA Reserved%'
    and releasedate > '2018-09-01'
    and cloud_sessions.acc in (select acc from dbgaps)
    group by cloud_sessions.acc;

create temp table public_sum as
select date_trunc('day',delay) as delay, count(*) as public_count
from first_public
group by date_trunc('day',delay);

create temp table cloud_sum as
select date_trunc('day',delay) as delay, count(*) as cloud_count
from first_cloud
group by date_trunc('day',delay);

create temp table dbgap_sum as
select date_trunc('day',delay) as delay, count(*) as dbgap_count
from first_dbgap
group by date_trunc('day',delay);

select public_sum.delay, public_count, cloud_count, dbgap_count
from public_sum
full outer join cloud_sum on public_sum.delay=cloud_sum.delay
full outer join dbgap_sum on public_sum.delay=dbgap_sum.delay
order by delay;



create temp table usage_dbgap as
select cloud_sessions.acc, date_trunc('day', start_ts) as day,
releasedate as released, count(*) as downloads
from cloud_sessions, public_ts
where
cloud_sessions.acc=public_ts.acc and
( cmds like '%GET%' or cmds like '%HEAD%' ) and
domain not like '%nih.gov%' and
domain not like '%IANA Reserved%' and
cloud_sessions.acc ~ '^[DES]RR[\d\.]{6,10}'
and releasedate > '2018-09-01'
--and greatest(releasedate, loaddate) > '2018-09-01'
and cloud_sessions.acc in (select acc from dbgaps)
group by cloud_sessions.acc, day, released;

create temp table usage_public as
select cloud_sessions.acc, date_trunc('day', start_ts) as day,
releasedate as released, count(*) as downloads
from cloud_sessions, public_ts
where
cloud_sessions.acc=public_ts.acc and
( cmds like '%GET%' or cmds like '%HEAD%' ) and
domain not like '%nih.gov%' and
domain not like '%IANA Reserved%' and
cloud_sessions.acc ~ '^[DES]RR[\d\.]{6,10}'
and releasedate > '2018-09-01'
-- and greatest(releasedate, loaddate) > '2018-09-01'
and cloud_sessions.acc not in (select acc from dbgaps)
group by cloud_sessions.acc, day, released;

create temp table usage_cloud as
select cloud_sessions.acc, date_trunc('day', start_ts) as day,
releasedate as released, count(*) as downloads
from cloud_sessions, public_ts
where
cloud_sessions.acc=public_ts.acc and
( cmds like '%GET%' or cmds like '%HEAD%' ) and
(domain like '%(AWS %)' or domain like '%(GCP%)') and
domain not like '%nih.gov%' and
domain not like '%IANA Reserved%' and
cloud_sessions.acc ~ '^[DES]RR[\d\.]{6,10}'
and cloud_sessions.acc not in (select acc from dbgaps)
and releasedate > '2018-09-01'
--greatest(releasedate, loaddate) > '2018-09-01'
group by cloud_sessions.acc, day, released;


create temp table total_dbgap as
select sum(downloads) as downloads,
min(date_trunc('day', day-released)) as days_since
--min(day) as first_download
from usage_dbgap
group by days_since
order by days_since;

create temp table total_public as
select sum(downloads) as downloads,
min(date_trunc('day', day-released)) as days_since
from usage_public
group by days_since
order by days_since;

create temp table total_cloud as
select sum(downloads) as downloads,
min(date_trunc('day', day-released)) as days_since
from usage_cloud
group by days_since
order by days_since;

select sum(downloads) from total_dbgap where days_since between  0 and 30;
select sum(downloads) from total_dbgap where days_since between 30 and 60;
select sum(downloads) from total_dbgap where days_since between 60 and 90;

select sum(downloads) from total_public where days_since between  0 and 30;
select sum(downloads) from total_public where days_since between 30 and 60;
select sum(downloads) from total_public where days_since between 60 and 90;

select delay, count(*) from (
select date_part('days', day-released) as delay from usage_public ) as foo
group by delay
order by delay;

select delay, count(*) from (
select date_part('days', day-released) as delay from usage_cloud) as foo
group by delay
order by delay;

select taxid || ': ' || scientificname, count(*), count(distinct ip)
from cloud_sessions, public
where acc not in (select acc from dbgaps) and
cloud_sessions.acc=public.run and
domain not like '%nih.gov%' and domain not like '%IANA Reserved%'
group by taxid || ': ' || scientificname
order by count(*) desc
limit 50;

select total_public.days_since,
total_public.downloads as public_downloads,
total_cloud.downloads as cloud_downloads,
total_dbgap.downloads as dbgap_downloads
from total_public
full outer join total_cloud on total_public.days_since=total_cloud.days_since
full outer join total_dbgap on total_public.days_since=total_dbgap.days_since
order by total_public.days_since;

create temp table downloads as
select
    days_since,
    'public' as source,
    downloads
    from total_public
union all
select
    days_since,
    'cloud' as source,
    downloads
    from total_cloud
union all
select
    days_since,
    'dbgap' as source,
    downloads
    from total_dbgap

\a
\o cold.csv
\f ','
\t on
select acc, ip, date_trunc('day',start_ts), bytecount
from cloud_sessions
where bytecount > 0 and start_ts > '2010-01-01' and acc ~ '^[DES]RR[\d\.]{6,10}'
order by start_ts, acc;

for days in $(seq 180); do
    cat /tmp/cold.csv | ~/strides/cold.py $days >> ~/cold_simulation.csv &
    sleep 2
done
