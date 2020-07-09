#!/bin/bash
# shellcheck source=strides_env.sh
. ./strides_env.sh

#--format: <none|json|prettyjson|csv|sparse|pretty>
FORMAT="pretty"
if [ "$#" -eq 1 ]; then
    FORMAT=$1
fi

export CLOUDSDK_CORE_PROJECT="ncbi-logmon"
gcloud config set account 253716305623-compute@developer.gserviceaccount.com


bq -q query \
    --format "$FORMAT" \
    --use_legacy_sql=false \
    "select source, accepted, count(*) as parsed_count from (select source, accepted from strides_analytics.gs_parsed union all select source, accepted from strides_analytics.s3_parsed) group by source, accepted order by source"


bq -q query \
    --format "$FORMAT" \
    --use_legacy_sql=false \
    "select source, count(*) as external_records, sum(num_requests) as total_requests, sum(bytes_sent) total_bytes_sent from strides_analytics.summary_export where domain not like '%nih.gov%' group by source order by source "


bq -q query \
    --format "$FORMAT" \
    --use_legacy_sql=false \
    "select source, count(*) as internal_records, sum(num_requests) as total_requests, sum(bytes_sent) total_bytes_sent from strides_analytics.summary_export where domain like '%nih.gov%' group by source order by source "


bq -q query \
    --use_legacy_sql=false \
    --format "$FORMAT" \
    "select source, count(*) as records, regexp_extract(bucket,r' \(.+\)') as format, sum(num_requests) as total_requests, sum(bytes_sent) total_bytes_sent from strides_analytics.summary_export where domain not like '%nih.gov%' group by source, format order by source, format"


bq -q query \
    --use_legacy_sql=false \
    --format "$FORMAT" \
    "select datetime_trunc(start_ts, month) as month, source, count(*) as records, sum(num_requests) as total_requests, sum(bytes_sent) total_bytes_sent from strides_analytics.summary_export where domain not like '%nih.gov%' group by source, month order by month"


bq -q query \
    --use_legacy_sql=false \
    --format "$FORMAT" \
    "select source, domain, count(*) as records, sum(num_requests) as total_requests, sum(bytes_sent) total_bytes_sent from strides_analytics.summary_export group by source, domain order by total_bytes_sent desc limit 20"


bq -q query \
    --use_legacy_sql=false \
    --format "$FORMAT" \
    "select source, ScientificName, count(*) as records, sum(num_requests) as total_requests, sum(bytes_sent) total_bytes_sent from strides_analytics.summary_export group by ScientificName, source order by total_bytes_sent desc limit 20"


bq -q query \
    --use_legacy_sql=false \
    --format "$FORMAT" \
    "select case when domain like '%nih.gov%' then 'internal (NCBI)' else 'external' end as user, source, http_operations, http_statuses, count(*) as records, sum(num_requests) as total_requests, sum(bytes_sent) total_bytes_sent from strides_analytics.summary_export group by source, http_operations, http_statuses, user having records > 10000 order by user desc, total_bytes_sent desc limit 50"


bq -q query \
    --use_legacy_sql=false \
    --format "$FORMAT" \
    --max_rows 10000 \
    "select datetime_trunc(start_ts, day) as day, source, count(*) as records, sum(num_requests) as total_download_requests, sum(bytes_sent) total_bytes_downloaded from strides_analytics.summary_export where (http_operations like '%GET%' or http_operations like '%HEAD%' ) and domain not like '%nih.gov%' group by source, day order by source, day"


bq -q query \
    --use_legacy_sql=false \
    --format "$FORMAT" \
 "SELECT source, remote_ip as cloud_uploader, domain, bucket, country_code, http_statuses, count(distinct accession) as num_uploads FROM ncbi-logmon.strides_analytics.summary_export where http_operations  like '%P%' and http_statuses like '%20%' and domain not like '%nih.gov%' group by remote_ip, bucket, countrY_code, domain, http_statuses,source order by num_uploads desc"


bq -q query \
    --use_legacy_sql=false \
    --format "$FORMAT" \
 "SELECT source, bucket, count(distinct accession) as num_uploads FROM ncbi-logmon.strides_analytics.summary_export where http_operations  like '%P%' group by source, bucket order by bucket, source"


bq -q query \
    --use_legacy_sql=false \
    --format "$FORMAT" \
 "SELECT source, domain as unknown_domain, remote_ip, count(distinct accession) as num_accessions FROM ncbi-logmon.strides_analytics.summary_export where domain like '%known%' or domain is null  group by source, domain, remote_ip order by num_accessions desc limit 10"


bq -q query \
    --use_legacy_sql=false \
    --format "$FORMAT" \
    --max_rows 10000 \
"select day as missing_day, sum(s3_requests) as s3_requests, sum(gs_requests) as gs_requests from ( SELECT datetime_trunc(start_ts, day) as day, case when source='S3' then num_requests else 0 end as s3_requests, case when source='GS' then num_requests else 0 end as gs_requests from strides_analytics.summary_export where domain not like '%nih.gov%' and  (http_operations like '%GET%' or http_operations like '%HEAD%' ) ) group by day having s3_requests < 2000 or gs_requests < 2000 order by day"

bq -q query \
    --use_legacy_sql=false \
    --format "$FORMAT" \
    --max_rows 10000 \
"select day as day, sum(s3_requests) as s3_requests, sum(gs_requests) as gs_requests, nih from ( SELECT datetime_trunc(start_ts, day) as day, case when source='S3' then num_requests else 0 end as s3_requests, case when source='GS' then num_requests else 0 end as gs_requests, domain like '%nih.gov%' as nih from strides_analytics.summary_export where (http_operations like '%GET%' or http_operations like '%HEAD%' ) ) group by day, nih order by day"


bq -q query \
    --use_legacy_sql=false \
    --format "$FORMAT" \
    --max_rows 10000 \
"SELECT split(user_agent,' ')[safe_offset(0)] || split(user_agent,' ')[safe_offset(1)] || split(user_agent,' ')[safe_offset(2)] as trunc_agent, sum(num_requests) as sessions FROM strides_analytics.summary_export export where user_agent like '%toolkit%' group by trunc_agent order by sessions desc limit 50"
