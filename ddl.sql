SET synchronous_commit = off;

CREATE USER grafana with password 'grafana';

DROP TABLE IF EXISTS ACCS;
CREATE TABLE accs(
    acc_id SERIAL PRIMARY KEY,
    acc TEXT NOT NULL UNIQUE,
    acc_part TEXT,
    acc_desc TEXT
);
GRANT ALL PRIVILEGES ON TABLE ACCS TO GRAFANA;
GRANT ALL ON SEQUENCE accs_acc_id_seq to GRAFANA;

DROP TABLE IF EXISTS AGENTS;
CREATE TABLE agents(
    agent_id SERIAL PRIMARY KEY,
    agent TEXT NOT NULL UNIQUE
);
GRANT ALL PRIVILEGES ON TABLE AGENTS TO GRAFANA;
GRANT ALL ON SEQUENCE agents_agent_id_seq to GRAFANA;

DROP TABLE IF EXISTS DOMAINS;
CREATE TABLE domains(
    domain_id SERIAL PRIMARY KEY,
    domain TEXT NOT NULL UNIQUE
);
GRANT ALL PRIVILEGES ON TABLE DOMAINS TO GRAFANA;
GRANT ALL ON SEQUENCE domains_domain_id_seq to GRAFANA;

DROP TABLE IF EXISTS IPS;
CREATE TABLE ips(
    ip_id BIGSERIAL PRIMARY KEY,
    ipint DOUBLE PRECISION NOT NULL,
    ip TEXT NOT NULL UNIQUE
);
GRANT ALL PRIVILEGES ON TABLE IPS TO GRAFANA;
ALTER TABLE ips ALTER ip_id SET STATISTICS 1000;
GRANT ALL ON SEQUENCE ips_ip_id_seq to GRAFANA;
DROP STATISTICS IF EXISTS ips_stat;
CREATE STATISTICS IF NOT EXISTS ips_stat on ip_id, ipint, ip from ips;


DROP TABLE IF EXISTS CLOUD_IPS;
CREATE TABLE CLOUD_IPS(
    ip TEXT NOT NULL UNIQUE,
    domain TEXT NOT NULL);
\copy cloud_ips FROM cloud_ips.tsv WITH DELIMITER E'\t';


DROP TABLE IF EXISTS sources;
CREATE TABLE sources(
    source TEXT NOT NULL PRIMARY KEY
);
GRANT ALL PRIVILEGES ON TABLE SOURCES TO GRAFANA;

CREATE table rdns (
  ip  text PRIMARY KEY,
  domain text);
CLUSTER rdns USING rdns_pkey;

\copy rdns FROM ips.tsv WITH DELIMITER E'\t';
-- See https://confluence.ncbi.nlm.nih.gov/pages/viewpage.action?spaceKey=~camacho&title=BLAST+STRIDES+Databases
-- AND https://ncbi.github.io/blast-cloud/blastdb/available-blastdbs-gcp.html


CREATE TABLE ip2location_db11_ipv4(
    ip_from DOUBLE PRECISION NOT NULL,
    ip_to DOUBLE PRECISION NOT NULL,
    country_code CHARACTER(2) NOT NULL,
    country_name CHARACTER VARYING(64) NOT NULL,
    region_name CHARACTER VARYING(128) NOT NULL,
    city_name CHARACTER VARYING(128) NOT NULL,
    latitude REAL NOT NULL,
    longitude REAL NOT NULL,
    zip_code CHARACTER VARYING(30) NOT NULL,
    time_zone CHARACTER VARYING(8) NOT NULL,
    CONSTRAINT ip2location_db11_ipv4_pkey PRIMARY KEY (ip_from, ip_to)
);
DROP INDEX IF EXISTS ip2location_ipv4_range_gist;
CREATE INDEX ip2location_ipv4_range_gist
    ON ip2location_db11_ipv4
    USING gist ((box(point(ip_from,ip_from),point(ip_to,ip_to))) box_ops);

\COPY ip2location_db11_ipv4 FROM 'IP2LOCATION-LITE-DB11.CSV' WITH CSV QUOTE AS '"';
CLUSTER ip2location_db11_ipv4 using ip2location_db11_ipv4_pkey;
DROP STATISTICS IF EXISTS ip2location_stat;
DROP STATISTICS IF EXISTS ip2location_stat2;
CREATE STATISTICS IF NOT EXISTS ip2location_stat
    on city_name, zip_code from ip2location_db11_ipv4;

CREATE STATISTICS IF NOT EXISTS ip2location_stat2
    on ip_from, ip_to from ip2location_db11_ipv4;

CREATE TABLE ip2location_db11_ipv6(
   ip_from DOUBLE PRECISION NOT NULL,
   ip_to DOUBLE PRECISION NOT NULL,
   country_code CHARACTER(2) NOT NULL,
   country_name CHARACTER VARYING(64) NOT NULL,
   region_name CHARACTER VARYING(128) NOT NULL,
   city_name CHARACTER VARYING(128) NOT NULL,
   latitude REAL NOT NULL,
   longitude REAL NOT NULL,
   zip_code CHARACTER VARYING(30) NOT NULL,
   time_zone CHARACTER VARYING(8) NOT NULL,
   CONSTRAINT ip2location_db11_ipv6_pkey PRIMARY KEY (ip_from, ip_to)
);
\COPY ip2location_db11_ipv6 FROM 'IP2LOCATION-LITE-DB11.IPV6.CSV' WITH CSV QUOTE AS '"';
DROP INDEX IF EXISTS ip2location_ipv6_range_gist;
CREATE INDEX ip2location_ipv6_range_gist
    ON ip2location_db11_ipv6
    USING gist ((box(point(ip_from,ip_from),point(ip_to,ip_to))) box_ops);

CLUSTER ip2location_db11_ipv6 using ip2location_db11_ipv6_pkey;

DROP VIEW IF EXISTS ip2location_db11;
CREATE VIEW ip2location_db11 AS
    select ip_from,
           ip_to,
           country_code,
           country_name,
           region_name,
           city_name,
           latitude,
           longitude,
           zip_code,
           time_zone
    from ip2location_db11_ipv4
    union all
    select ip_from,
           ip_to,
           country_code,
           country_name,
           region_name,
           city_name,
           latitude,
           longitude,
           zip_code,
           time_zone
    from ip2location_db11_ipv6 where ip_from > 2^32;


CREATE TABLE egress_cost (country_code CHARACTER(2) PRIMARY KEY, price REAL default 0.08);
INSERT INTO egress_cost (country_code)
    select DISTINCT  country_code FROM ip2location_db11;
UPDATE egress_cost SET price=0.06 WHERE country_code IN ('US', 'CA');
UPDATE egress_cost SET price=0.15 WHERE country_code IN ('AU', 'NZ');
UPDATE egress_cost SET price=0.19 WHERE country_code='CN';

-- Obtain FROM https://www.ncbi.nlm.nih.gov/sra/?term=%22viruses%22%5Borgn%5D,
-- use Spreadsheet for query search terms
-- TODO: Use NULL/FORCE_NOT_NULL for  Total_Spots/Bases?

DROP TABLE IF EXISTS sra_dump;
CREATE TABLE sra_dump
(
    Run text PRIMARY KEY,
    ReleaseDate text,
    LoadDate text,
    spots text,
    bases text,
    spots_with_mates text,
    avgLength text,
    size_MB text,
    AssemblyName text,
    download_path text,
    Experiment text,
    LibraryName text,
    LibraryStrategy text,
    LibrarySelection text,
    LibrarySource text,
    LibraryLayout text,
    InsertSize text,
    InsertDev text,
    Platform text,
    Model text,
    SRAStudy text,
    BioProject text,
    Study_Pubmed_id text,
    ProjectID text,
    Sample text,
    BioSample text,
    SampleType text,
    TaxID text,
    ScientificName text,
    SampleName text,
    g1k_pop_code text,
    source text,
    g1k_analysis_group text,
    Subject_ID text,
    Sex text,
    Disease text,
    Tumor text,
    Affection_Status text,
    Analyte_Type text,
    Histological_Type text,
    Body_Site text,
    CenterName text,
    Submission text,
    dbgap_study_accession text,
    Consent text,
    RunHash text,
    ReadHash text
);
DROP TABLE IF EXISTS rna_seq;
CREATE TABLE rna_seq (LIKE sra_dump INCLUDING INDEXES);
\COPY rna_seq FROM 'rna-seq.csv' WITH CSV HEADER QUOTE AS '"' NULL ''
DROP TABLE IF EXISTS foodborne;
CREATE TABLE foodborne (LIKE sra_dump INCLUDING INDEXES);
\COPY foodborne FROM 'foodborne.csv' WITH CSV HEADER QUOTE AS '"'
DROP TABLE IF EXISTS carb;
CREATE TABLE carb (LIKE sra_dump INCLUDING INDEXES);
\COPY carb FROM 'carb.csv' WITH CSV HEADER QUOTE AS '"'
DROP TABLE IF EXISTS metagenomic;
CREATE TABLE metagenomic (LIKE sra_dump INCLUDING INDEXES);
\COPY metagenomic FROM 'metagenomes.csv' WITH CSV HEADER QUOTE AS '"'
DROP TABLE IF EXISTS viral;
CREATE TABLE viral (LIKE sra_dump INCLUDING INDEXES);
\COPY viral FROM 'viral.csv' WITH CSV HEADER QUOTE AS '"'
DROP TABLE IF EXISTS public;
CREATE TABLE public (LIKE sra_dump INCLUDING INDEXES);
\COPY public FROM 'public_fix.csv' WITH CSV HEADER QUOTE AS '"'
CLUSTER public USING public_pkey;

DROP TABLE IF EXISTS categories;
CREATE table categories as
    select experiment_accession, organism_name, 'Metagenomic' AS category
        FROM metagenomic union all
    select experiment_accession, organism_name, 'CARB' AS category
        FROM carb union all
    select experiment_accession, organism_name, 'RNA_seq' AS category
        FROM rna_seq union all
    select experiment_accession, organism_name, 'Foodborne' AS category
        FROM foodborne union all
    select experiment_accession, organism_name, 'Viral' AS category
        FROM viral union all
    select experiment_accession, organism_name, 'Public' AS category
        FROM public
        ;
CREATE index categories_acc on categories (experiment_accession);


DROP TABLE IF EXISTS blast_accesses;
/*
CREATE TABLE blast_accesses(
  ip_id DOUBLE PRECISION REFERENCES ips,
  acc_id INTEGER REFERENCES accs,
  agent_id INTEGER REFERENCES agents,
  domain_id INTEGER REFERENCES domains,
  res INT,
  start_ts TIMESTAMP,
  end_ts TIMESTAMP,
  bytes DOUBLE PRECISION,
  cnt INT,
  PRIMARY KEY (start_ts, acc_id, ip_id, agent_id, res)
);
ALTER TABLE blast_accesses ALTER ip_id SET STATISTICS 1000;
DROP INDEX IF EXISTS blast_accesses_start;
CLUSTER blast_accesses USING blast_accesses_pkey;
DROP INDEX IF EXISTS blast_accesses_acc_id;
CREATE INDEX blast_accesses_acc_id on blast_accesses(acc_id);
GRANT ALL PRIVILEGES ON TABLE BLAST_ACCESSES TO GRAFANA;
*/
DROP TABLE IF EXISTS sra_sessions CASCADE;
/*
CREATE TABLE sra_sessions(
  ip_id DOUBLE PRECISION REFERENCES ips,
  acc_id INTEGER REFERENCES accs,
  agent_id INTEGER REFERENCES agents,
  domain_id INTEGER REFERENCES domains,
  res INT NOT NULL,
  start_ts TIMESTAMP,
  end_ts TIMESTAMP,
  bytes DOUBLE PRECISION NOT NULL,
  cnt INT NOT NULL,
  CREATED TIMESTAMP NOT NULL,
  UPDATED TIMESTAMP NOT NULL,
  PRIMARY KEY (start_ts, acc_id, ip_id, agent_id, res)
);
DROP INDEX IF EXISTS sra_sessions_start;
CLUSTER sra_sessions USING sra_sessions_pkey;
DROP INDEX IF EXISTS sra_sessions_acc_id;
CREATE INDEX sra_sessions_acc_id ON sra_sessions(acc_id);
GRANT ALL PRIVILEGES ON TABLE sra_sessions TO GRAFANA;
*/

DROP TABLE IF EXISTS sessions CASCADE;
CREATE TABLE sessions(
  start_ts TIMESTAMP NOT NULL,
  end_ts TIMESTAMP NOT NULL,
  bytes DOUBLE PRECISION NOT NULL,
  ip_id DOUBLE PRECISION REFERENCES ips,
  acc_id INTEGER REFERENCES accs,
  agent_id INTEGER REFERENCES agents,
  domain_id INTEGER REFERENCES domains NOT NULL,
  res INT NOT NULL,
  cnt INT NOT NULL,
  CREATED TIMESTAMP NOT NULL,
  UPDATED TIMESTAMP NOT NULL,
  source TEXT REFERENCES sources,
  PRIMARY KEY (start_ts, acc_id, ip_id, agent_id, res, source)
);
ALTER TABLE sessions ALTER ip_id SET STATISTICS 1000;
ALTER TABLE sessions ALTER acc_id SET STATISTICS 1000;
ALTER TABLE sessions ALTER start_ts SET STATISTICS 1000;
DROP INDEX IF EXISTS sessions_start;
CLUSTER sessions USING sessions_pkey;
DROP INDEX IF EXISTS sessions_acc_id;
CREATE INDEX sessions_acc_id ON sessions(acc_id);
GRANT ALL PRIVILEGES ON TABLE sessions TO GRAFANA;
DROP STATISTICS IF EXISTS sessions_stat;
DROP STATISTICS IF EXISTS sessions_stat2;
DROP STATISTICS IF EXISTS sessions_stat3;
CREATE STATISTICS IF NOT EXISTS sessions_stat
    on ip_id, domain_id, agent_id from sessions;
CREATE STATISTICS IF NOT EXISTS sessions_stat2
    on source, acc_id, ip_id from sessions;
CREATE STATISTICS IF NOT EXISTS sessions_stat3
    on source, agent_id, ip_id from sessions;




DROP VIEW IF EXISTS blast_usage;
DROP VIEW IF EXISTS blast_sessions;
DROP VIEW IF EXISTS sra_usage;
DROP VIEW IF EXISTS sra_sessions;

DROP VIEW IF EXISTS sra_cloud;
DROP VIEW IF EXISTS sra_cloud_usage;

/*
DROP MATERIALIZED VIEW IF EXISTS blast_loc;
DROP VIEW IF EXISTS blast_ip_hit;
DROP VIEW IF EXISTS blast_sessions_tmp;

CREATE VIEW blast_sessions_tmp AS
    SELECT start_ts, end_ts, acc, acc_desc, acc_part, bytes,
           cnt, ips.ip as ip, rdns.domain as domain, ipint::DOUBLE PRECISION
    FROM blast_accesses
        JOIN ips USING (ip_id)
        JOIN accs USING (acc_id)
        JOIN rdns USING (ip);

CREATE VIEW blast_ip_hit AS
    SELECT distinct ipint
    FROM blast_sessions_tmp;

CREATE MATERIALIZED VIEW blast_loc AS
    SELECT ipint, country_code, city_name
    FROM ip2location_db11, blast_ip_hit
    WHERE blast_ip_hit.ipint BETWEEN ip_from AND ip_to;
DROP INDEX IF EXISTS blast_loc_pkey;
CREATE UNIQUE INDEX blast_loc_pkey on blast_loc (ipint);

CREATE VIEW blast_sessions AS
    SELECT blast_sessions_tmp.*, city_name, country_code
    FROM blast_sessions_tmp
        JOIN blast_loc USING (ipint);

REFRESH MATERIALIZED VIEW CONCURRENTLY blast_loc;
*/

CREATE OR REPLACE VIEW blast_sessions AS
SELECT start_ts, source, end_ts, acc, acc_desc, acc_part, bytes,
           cnt, ips.ip as ip, rdns.domain as domain, ipint,
           city_name, country_code, agent, res
    FROM sessions, ips, ip2location_db11, accs, rdns, agents
        WHERE
            source like 'blast%' AND
            --(acc like '%_v5%' OR acc like 'GCF%') AND
            sessions.ip_id=ips.ip_id AND
            sessions.acc_id = accs.acc_id AND
            ips.ip=rdns.ip AND
            sessions.agent_id=agents.agent_id AND
            box(point(ip_from,ip_from),point(ip_to,ip_to)) @>
            box(point (ipint,ipint), point(ipint,ipint)) ;

CREATE OR REPLACE VIEW blast_usage AS
    SELECT date_trunc('day', start_ts) AS time,
    concat(acc_desc,' (',acc_part,')') as db,
    SUM(cnt) AS cnt, sum(bytes) as bytes
    FROM blast_sessions
    GROUP BY time, db;

CREATE OR REPLACE VIEW sra_sessions AS
SELECT start_ts, source, end_ts, acc, acc_desc, acc_part, bytes,
           cnt, ips.ip as ip, rdns.domain as domain, ipint,
           city_name, country_code, agent, res
    FROM sessions, ips, ip2location_db11, accs, rdns, agents
        WHERE
            source like 'sra%' AND
            sessions.ip_id=ips.ip_id AND
            sessions.acc_id = accs.acc_id AND
            ips.ip=rdns.ip AND
            sessions.agent_id=agents.agent_id AND
            box(point(ip_from,ip_from),point(ip_to,ip_to)) @>
            box(point (ipint,ipint), point(ipint,ipint)) ;

CREATE OR REPLACE VIEW sra_usage AS
    SELECT date_trunc('hour', start_ts) AS time, acc_part AS db,
    SUM(cnt) AS cnt
    FROM sra_sessions
    GROUP BY time, db;


CREATE OR REPLACE VIEW blast_res AS
SELECT  source, acc, acc_desc, acc_part, ip, domain,
        city_name, country_code, agent,
      MIN(start_ts) as start_ts, MAX(end_ts) as end_ts,
      SUM(bytes) as bytes, SUM(cnt) as cnt,
      array_agg(res) as res,
      array_agg(start_ts) as starts
FROM blast_sessions
GROUP by source, acc, acc_desc, acc_part, ip, domain,
        city_name, country_code, agent;

CREATE OR REPLACE VIEW sra_res AS
SELECT  source, acc, acc_desc, acc_part, ip, domain,
        city_name, country_code, agent,
      MIN(start_ts) as start_ts, MAX(end_ts) as end_ts,
      SUM(bytes) as bytes, SUM(cnt) as cnt,
      array_agg(res) as res,
      array_agg(start_ts) as starts
FROM sra_sessions
GROUP by source, acc, acc_desc, acc_part, ip, domain,
        city_name, country_code, agent;


DROP TABLE IF EXISTS storage CASCADE;
CREATE TABLE storage (
  start_ts TIMESTAMP NOT NULL,
  bytes DOUBLE PRECISION NOT NULL,
  bucket TEXT,
  source TEXT REFERENCES sources,
  CREATED TIMESTAMP NOT NULL,
  UPDATED TIMESTAMP NOT NULL,
  PRIMARY KEY (start_ts, source, bucket)
);
CLUSTER storage USING storage_pkey;
GRANT ALL PRIVILEGES ON TABLE storage TO GRAFANA;

DROP TABLE IF EXISTS bigqueries CASCADE;
CREATE TABLE bigqueries (
    start_ts TIMESTAMP NOT NULL,
    bytes DOUBLE PRECISION NOT NULL,
    tables text NOT NULL,
    source TEXT REFERENCES sources,
    username text NOT NULL,
    CREATED TIMESTAMP NOT NULL,
    UPDATED TIMESTAMP NOT NULL,
    PRIMARY KEY (start_ts, source, tables)
);
CLUSTER bigqueries USING bigqueries_pkey;
GRANT ALL PRIVILEGES ON TABLE bigqueries TO GRAFANA;

GRANT ALL ON DATABASE GRAFANA TO GRAFANA;

CLUSTER; VACUUM ANALYZE;

CREATE OR REPLACE VIEW sra_cloud AS
SELECT start_ts, source, end_ts, acc, acc_desc, acc_part, bytes,
           cnt, ips.ip as ip, rdns.domain as domain, ipint,
           city_name, country_code, agent, res
    FROM sessions, ips, ip2location_db11, accs, rdns, agents
        WHERE
            source like 'SRA@%' AND
            sessions.ip_id=ips.ip_id AND
            sessions.acc_id = accs.acc_id AND
            ips.ip=rdns.ip AND
            sessions.agent_id=agents.agent_id AND
            box(point(ip_from,ip_from),point(ip_to,ip_to)) @>
            box(point (ipint,ipint), point(ipint,ipint)) ;

CREATE OR REPLACE VIEW sra_cloud_usage AS
    SELECT date_trunc('hour', start_ts) AS time, acc_part AS db,
    SUM(cnt) AS cnt
    FROM sra_cloud
    GROUP BY time, db;

CREATE OR REPLACE VIEW sra_last_used AS
    SELECT acc as acc,
    max(date_trunc('day', start_ts)) as last
    from sessions, accs
    where sessions.acc_id=accs.acc_id group by acc
    UNION ALL
    SELECT run as acc, '2018-11-01 00:00:00'::timestamp as last
    from public;


delete from sessions where cnt > 10000 and bytes/cnt > 1048576;

DROP TABLE IF EXISTS export;
CREATE TABLE export (
    status text,
    ip text,
    domain text,
    cmds text,
    bytecount bigint,
    agent text,
    cnt int,
    acc text,
    start timestamp,
    "end" timestamp,
    source text
    --PRIMARY KEY (ip, acc, agent, source, domain, "start")
);

CREATE index export_start on export (start);
CREATE index export_source on export (source);
CLUSTER export using export_start;


create or replace view export_test as
select status, export.ip as ip , rdns.domain, cmds, bytecount,
      agent, cnt, acc, start, "end", source, ipint, city_name, country_code,
    ipint
from export, ips, rdns, ip2location_db11
where
    box(point(ip_from,ip_from),point(ip_to,ip_to)) @>
    box(point (ipint,ipint), point(ipint,ipint))

