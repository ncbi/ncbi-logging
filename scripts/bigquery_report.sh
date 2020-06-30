#!/bin/bash
# shellcheck source=strides_env.sh
. ./strides_env.sh

export CLOUDSDK_CORE_PROJECT="ncbi-logmon"
gcloud config set account 253716305623-compute@developer.gserviceaccount.com


bq -q query \
    --use_legacy_sql=false \
    "select source, count(*) as records, sum(num_requests) as total_requests, sum(bytes_sent) total_bytes_sent from strides_analytics.summary_export where domain not like '%NLM%' group by source order by source "


bq -q query \
    --use_legacy_sql=false \
    "select source, count(*) as records, regexp_extract(bucket,r' \(.+\)') as format, sum(num_requests) as total_requests, sum(bytes_sent) total_bytes_sent from strides_analytics.summary_export where domain not like '%NLM%' group by source, format order by source, format"


bq -q query \
    --use_legacy_sql=false \
    "select datetime_trunc(start_ts, month) as month, source, count(*) as records, sum(num_requests) as total_requests, sum(bytes_sent) total_bytes_sent from strides_analytics.summary_export where domain not like '%NLM%' group by source, month order by month, source"

bq -q query \
    --use_legacy_sql=false \
    "select source, domain, count(*) as records, sum(num_requests) as total_requests, sum(bytes_sent) total_bytes_sent from strides_analytics.summary_export group by source, domain order by records desc limit 20"

bq -q query \
    --use_legacy_sql=false \
    "select source, domain, count(*) as records, sum(num_requests) as total_requests, sum(bytes_sent) total_bytes_sent from strides_analytics.summary_export where domain like '%AWS%' or domain like '%GCP%' or domain like '%.edu' or domain like '%.gov' or domain like '%.nih.gov' or domain like '%.nlm.nih.gov' group by source, domain order by records desc"

bq -q query \
    --use_legacy_sql=false \
    "select source, ScientificName, count(*) as records, sum(num_requests) as total_requests, sum(bytes_sent) total_bytes_sent from strides_analytics.summary_export group by ScientificName, source order by records desc limit 20"

bq -q query \
    --use_legacy_sql=false \
    "select source, http_operations, http_statuses, count(*) as records, sum(num_requests) as total_requests, sum(bytes_sent) total_bytes_sent from strides_analytics.summary_export group by source, http_operations, http_statuses order by records desc limit 50"
