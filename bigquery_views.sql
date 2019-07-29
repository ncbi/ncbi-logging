CREATE OR REPLACE VIEW
  `ncbi-sandbox-blast.strides_analytics.gs_fix` AS
SELECT
  ARRAY_TO_STRING( ARRAY_AGG( DISTINCT CAST(sc_status AS string)), " ") AS status,
  c_ip AS ip,
  cs_bucket AS domain,
  ARRAY_TO_STRING( ARRAY_AGG(DISTINCT cs_method), " ") AS cmds,
  SUM(sc_bytes) AS bytecount,
  MIN(TIMESTAMP_MILLIS(CAST(time_micros/1000 AS int64))) AS `start`,
  MAX(TIMESTAMP_MILLIS(CAST((time_micros+time_taken_micros)/ 1000 AS int64))) AS `end`,
  REPLACE(SUBSTR(cs_user_agent, 0, 64),",gzip(gfe)","") AS agent,
  COUNT(*) AS cnt,
  REGEXP_EXTRACT(cs_uri, r"[DES]RR[0-9]{6,8}") AS acc,
  'GS' AS source
FROM
  `ncbi-sandbox-blast.strides_analytics.gs_prod`
GROUP BY
  ip,
  acc,
  agent,
  source,
  domain

CREATE OR REPLACE VIEW
  `ncbi-sandbox-blast.strides_analytics.sra_fix` AS
SELECT
  status,
  ip,
  domain,
  cmds,
  bytecount,
  agent,
  cnt,
  substr(acc,0,100) as acc,
  TIMESTAMP_MILLIS(CAST(`start`*1000 AS int64)) AS `start`,
  TIMESTAMP_MILLIS(CAST(`end`*1000 AS int64)) AS `end`,
  'SRA' AS source
FROM
  `ncbi-sandbox-blast.strides_analytics.sra_prod`

CREATE OR REPLACE VIEW
  `ncbi-sandbox-blast.strides_analytics.s3_fix` AS
SELECT
  status,
  ip,
  domain,
  cmds,
  bytecount,
  agent,
  cnt,
  substr(acc,0,100) as acc,
  TIMESTAMP_MILLIS(CAST(`start`*1000 AS int64)) AS `start`,
  TIMESTAMP_MILLIS(CAST(`end`*1000 AS int64)) AS `end`,
  'S3' AS source
FROM
  `ncbi-sandbox-blast.strides_analytics.s3_prod`

CREATE OR REPLACE VIEW `ncbi-sandbox-blast.strides_analytics.combined` AS
    SELECT
    status,
    ip,
    domain,
    cmds,
    bytecount,
    agent,
    cnt,
    acc,
    `start`,
    `end`,
    source
    from
    `ncbi-sandbox-blast.strides_analytics.gs_fix`
    UNION ALL SELECT
    status,
    ip,
    domain,
    cmds,
    bytecount,
    agent,
    cnt,
    acc,
    `start`,
    `end`,
    source
    from
    `ncbi-sandbox-blast.strides_analytics.sra_fix`
    UNION ALL SELECT
    status,
    ip,
    domain,
    cmds,
    bytecount,
    agent,
    cnt,
    acc,
    `start`,
    `end`,
    source
    from
    `ncbi-sandbox-blast.strides_analytics.s3_fix`

CREATE OR REPLACE TABLE
  `ncbi-sandbox-blast.strides_analytics.export` AS
SELECT
  status,
  ip,
  domain,
  cmds,
  bytecount,
  agent,
  cnt,
  acc,
  start,
  `end`,
  source,
IF
  (REGEXP_CONTAINS(ip, ":"),
    0,
    NET.IPV4_TO_INT64(NET.SAFE_IP_FROM_STRING(ip)) ) AS ipint
FROM
  `ncbi-sandbox-blast.strides_analytics.combined`
    
    --bq rm -f strides_analytics.export


    SELECT
  DISTINCT ip,
  country_name,
  NET.SAFE_IP_FROM_STRING(ip) & NET.IP_NET_MASK(4,
    mask) AS network_bin
FROM
  `ncbi-sandbox-blast.strides_analytics.export`,
  `fh-bigquery.geocode.201806_geolite2_city_ipv4_locs`
WHERE
  network_bin=mask
