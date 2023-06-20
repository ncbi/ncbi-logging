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

/opt/panfs/bin/pan_df -H /panfs/traces01.be-md.ncbi.nlm.nih.gov/strides-analytics/

#bq -q query \
#    --format "$FORMAT" \
#    --use_legacy_sql=false \
#    "select source, accepted, count(*) as parsed_count from (select source, accepted from $DATASET.gs_parsed union all select source, accepted from $DATASET.s3_parsed) group by source, accepted order by source"

echo "$DATASET"

if [ "$STRIDES_SCOPE" == "private" ]; then
    REQUEST_CUTOFF=100
else
    REQUEST_CUTOFF=1000
fi

bq -q query \
    --use_legacy_sql=false \
    --format "$FORMAT" \
    "WITH day_counts AS ( select day, sum(s3_requests) as s3_requests, sum(gs_requests) as gs_requests, sum(op_requests) as op_requests from ( SELECT datetime_trunc(start_ts, day) as day, case when source='S3' then num_requests else 0 end as s3_requests, case when source='GS' then num_requests else 0 end as gs_requests, case when source='OP' then num_requests else 0 end as op_requests FROM $DATASET.summary_export where (http_operations like '%GET%' or http_operations like '%HEAD%' ) and start_ts > '2021-01-01' ) group by day having s3_requests <  $REQUEST_CUTOFF or gs_requests < $REQUEST_CUTOFF or op_requests < $REQUEST_CUTOFF ), pop AS ( SELECT * FROM UNNEST(GENERATE_DATE_ARRAY('2021-01-01', current_date(), INTERVAL 1 DAY)) AS missing_day) select missing_day, gs_requests, s3_requests, op_requests from day_counts, pop where missing_day=day order by day desc "

#bq -q query \
#    --format "$FORMAT" \
#    --use_legacy_sql=false \
#    "select max(datetime_trunc(start_ts, day)) as gs_max_day from $DATASET.gs_fixed"

#bq -q query \
#    --format "$FORMAT" \
#    --use_legacy_sql=false \
#    "select max(datetime_trunc(start_ts, day)) as op_max_day from $DATASET.op_fixed"

#bq -q query \
#    --format "$FORMAT" \
#    --use_legacy_sql=false \
#    "select max(datetime_trunc(start_ts, day)) as s3_max_day from $DATASET.s3_fixed"

#bq -q query \
#    --format "$FORMAT" \
#    --use_legacy_sql=false \
#    "select source, count(*) as bad_dates from $DATASET.detail_export where start_ts < '2011-01-01' group by source"

bq -q query \
    --format "$FORMAT" \
    --use_legacy_sql=false \
    "select table_id, row_count, size_bytes FROM $DATASET.__TABLES__ order by size_bytes desc"

bq -q query \
    --format "$FORMAT" \
    --use_legacy_sql=false \
    "select source, count(*) as external_records, sum(num_requests) as total_requests, sum(bytes_sent) total_bytes_sent from $DATASET.summary_export where domain not like '%nih.gov%' group by source order by source "

bq -q query \
    --format "$FORMAT" \
    --use_legacy_sql=false \
    "select source, count(*) as internal_records, sum(num_requests) as total_requests, sum(bytes_sent) total_bytes_sent from $DATASET.summary_export where domain like '%nih.gov%' group by source order by source "

bq -q query \
    --use_legacy_sql=false \
    --format "$FORMAT" \
    "select source, count(*) as records, regexp_extract(bucket,r' \(.+\)') as format, sum(num_requests) as total_requests, sum(bytes_sent) total_bytes_sent from $DATASET.summary_export where domain not like '%nih.gov%' group by source, format order by source, format"

bq -q query \
    --use_legacy_sql=false \
    --format "$FORMAT" \
    "select datetime_trunc(start_ts, month) as month, source, count(*) as records, sum(num_requests) as total_requests, sum(bytes_sent) total_bytes_sent from $DATASET.summary_export where domain not like '%nih.gov%' group by source, month order by month desc"

bq -q query \
    --use_legacy_sql=false \
    --format "$FORMAT" \
    "select source, domain, count(*) as records, sum(num_requests) as total_requests, sum(bytes_sent) total_bytes_sent from $DATASET.summary_export group by source, domain order by total_bytes_sent desc limit 20"

bq -q query \
    --use_legacy_sql=false \
    --format "$FORMAT" \
    "select source, ScientificName, count(*) as records, sum(num_requests) as total_requests, sum(bytes_sent) total_bytes_sent from $DATASET.summary_export group by ScientificName, source order by total_bytes_sent desc limit 20"

bq -q query \
    --use_legacy_sql=false \
    --format "$FORMAT" \
    "select case when domain like '%nih.gov%' then 'internal (NCBI)' else 'external' end as user, source, http_operations, http_statuses, count(*) as records, sum(num_requests) as total_requests, sum(bytes_sent) total_bytes_sent from $DATASET.summary_export group by source, http_operations, http_statuses, user having records > 10000 order by user desc, total_bytes_sent desc limit 50"

bq -q query \
    --use_legacy_sql=false \
    --format "$FORMAT" \
    --max_rows 10000 \
    "select datetime_trunc(start_ts, day) as day, source, count(*) as records, sum(num_requests) as total_download_requests, sum(bytes_sent) total_bytes_downloaded from $DATASET.summary_export where (http_operations like '%GET%' or http_operations like '%HEAD%' ) and domain not like '%nih.gov%' and start_ts >= '2022-06-01' group by source, day order by day desc, source limit 50"

bq -q query \
    --use_legacy_sql=false \
    --format "$FORMAT" \
    "SELECT source, remote_ip as cloud_uploader, domain, bucket, country_code, http_statuses, count(distinct accession) as num_uploads FROM ncbi-logmon.$DATASET.summary_export where http_operations  like '%P%' and http_statuses like '%20%' and domain not like '%nih.gov%' group by remote_ip, bucket, countrY_code, domain, http_statuses,source order by num_uploads desc"

bq -q query \
    --use_legacy_sql=false \
    --format "$FORMAT" \
    "SELECT source, bucket, count(distinct accession) as num_uploads FROM ncbi-logmon.$DATASET.summary_export where http_operations  like '%P%' group by source, bucket order by bucket, source"

bq -q query \
    --use_legacy_sql=false \
    --format "$FORMAT" \
    "SELECT source, domain as unknown_domain, remote_ip, count(distinct accession) as num_accessions FROM ncbi-logmon.$DATASET.summary_export where domain like '%known%' or domain is null  group by source, domain, remote_ip order by num_accessions desc limit 10"

bq -q query \
    --use_legacy_sql=false \
    --format "$FORMAT" \
    --max_rows 10000 \
    "select day as missing_day, sum(s3_requests) as s3_requests, sum(gs_requests) as gs_requests from ( SELECT datetime_trunc(start_ts, day) as day, case when source='S3' then num_requests else 0 end as s3_requests, case when source='GS' then num_requests else 0 end as gs_requests from $DATASET.summary_export where (http_operations like '%GET%' or http_operations like '%HEAD%' ) ) group by day having s3_requests < 2000 or gs_requests < 2000 order by day desc limit 100"

bq -q query \
    --use_legacy_sql=false \
    --format "$FORMAT" \
    --max_rows 10000 \
    "select day as day, sum(s3_requests) as s3_requests, sum(gs_requests) as gs_requests, nih from ( SELECT datetime_trunc(start_ts, day) as day, case when source='S3' then num_requests else 0 end as s3_requests, case when source='GS' then num_requests else 0 end as gs_requests, domain like '%nih.gov%' as nih from $DATASET.summary_export where (http_operations like '%GET%' or http_operations like '%HEAD%' ) ) group by day, nih order by day desc limit 100"

bq -q query \
    --use_legacy_sql=false \
    --format "$FORMAT" \
    --max_rows 10000 \
    "select day as day, sum(s3_requests) as s3_requests, sum(gs_requests) as gs_requests from ( SELECT datetime_trunc(start_ts, day) as day, case when source='S3' then num_requests else 0 end as s3_requests, case when source='GS' then num_requests else 0 end as gs_requests from $DATASET.summary_export where domain not like '%nih.gov%' and (http_operations like '%GET%' or http_operations like '%HEAD%' ) ) group by day order by day desc"

bq -q query \
    --use_legacy_sql=false \
    --format "$FORMAT" \
    --max_rows 10000 \
    "SELECT split(user_agent,' ')[safe_offset(0)] || split(user_agent,' ')[safe_offset(1)] || split(user_agent,' ')[safe_offset(2)] as trunc_agent, sum(num_requests) as sessions FROM $DATASET.summary_export export where user_agent like '%toolkit%' group by trunc_agent order by sessions desc limit 50"

bq -q query \
    --use_legacy_sql=false \
    --format "$FORMAT" \
    "SELECT remote_ip as possible_mirror, domain, source, count(distinct accession) as uniq_accs  FROM ncbi-logmon.$DATASET.summary_export where domain not like '%nih.gov%' and start_ts > '2020-05-01' group by remote_ip, source, domain order by uniq_accs desc limit 10"

bq -q query \
    --use_legacy_sql=false \
    --format "$FORMAT" \
    "SELECT source, domain, remote_ip, COUNT(DISTINCT accession) AS numacc FROM ncbi-logmon.$DATASET.summary_export WHERE start_ts > '2019-03-01' and domain not like '%nih.gov%' GROUP BY source, domain, remote_ip ORDER BY numacc DESC LIMIT 100"

bq -q query \
    --use_legacy_sql=false \
    --format "$FORMAT" \
    "SELECT source, domain, COUNT(DISTINCT accession) AS numacc FROM ncbi-logmon.$DATASET.summary_export WHERE start_ts > '2019-03-01' and domain not like '%nih.gov%' GROUP BY source, domain ORDER BY numacc DESC LIMIT 100"

bq -q query \
    --use_legacy_sql=false \
    --format "$FORMAT" \
    "select source, datetime_trunc(start_ts, day) as day, remote_ip as ip_lots_accessions, domain, count(distinct accession) AS num_accessions_busy FROM ncbi-logmon.$DATASET.summary_export WHERE source!='OP' and start_ts > '2019-03-01' and domain not like '%nih.gov%' GROUP BY day, source, domain, remote_ip having num_accessions_busy > 5000 ORDER BY day, source DESC LIMIT 1000"

bq -q query \
    --use_legacy_sql=false \
    --format "$FORMAT" \
    "select source, domain, count(distinct accession) as num_accessions FROM ncbi-logmon.$DATASET.summary_export WHERE start_ts > '2019-03-01' GROUP BY source, domain ORDER BY num_accessions desc limit 10"

bq -q query \
    --use_legacy_sql=false \
    --format "$FORMAT" \
    "select source, bucket , count(distinct accession) as num_accessions FROM ncbi-logmon.$DATASET.summary_export GROUP BY source, bucket ORDER BY num_accessions desc"

bq -q query \
    --use_legacy_sql=false \
    --format "$FORMAT" \
    "select remote_ip as null_city_ip, domain, count(distinct accession) as num_accessions FROM ncbi-logmon.$DATASET.summary_export where city_name='Unknown' or city_name is null GROUP BY remote_ip, domain ORDER BY num_accessions desc"

bq -q query \
    --use_legacy_sql=false \
    --format "$FORMAT" \
    "SELECT datetime_trunc(start_ts, month) as month, bucket_owner, user_location, SUM(bytes_sent) as total_bytes, COUNT(*) as num_accesses, count(distinct accession) as distinct_accessions FROM ( SELECT start_ts, accession, case when starts_with(bucket ,'sra-pub-src-1') then 'Public' when starts_with(bucket , 'sra-pub-src-2') then 'Public' else 'NCBI' end as bucket_owner, bytes_sent, CASE when domain like '%nih.gov%' THEN 'NCBI' WHEN domain LIKE '%AWS%' THEN 'AWS' ELSE 'External' END AS user_location FROM $DATASET.summary_export WHERE source='S3' AND http_operations NOT LIKE '%P%' ) GROUP BY month, bucket_owner, user_location order by month desc, bucket_owner, user_location"

bq -q query \
    --use_legacy_sql=false \
    --format "$FORMAT" \
    "select datetime_trunc(start_ts, day) as day, count(distinct remote_ip) as uniq_ips, sum(num_requests) as total_requests, sum(bytes_sent) total_bytes_sent from $DATASET.summary_export where domain not like '%nih.gov%' and source='S3' and bucket like '%cov2%' and start_ts > '2022-06-01' group by day order by day desc"

bq -q query \
    --use_legacy_sql=false \
    --format "$FORMAT" \
    "select datetime_trunc(start_ts, day) as day, count(distinct remote_ip) as uniq_ips_to_cloud from $DATASET.summary_export where domain not like '%nih.gov%' and (source='S3' or source='GS') group by day order by day desc"

bq -q query \
    --use_legacy_sql=false \
    --format "$FORMAT" \
    "select bucket, source, count(distinct remote_ip) as uniq_ips from $DATASET.summary_export where domain not like '%nih.gov%' and (source='S3' or source='GS') group by bucket, source order by uniq_ips desc"

bq -q query \
    --use_legacy_sql=false \
    --format "$FORMAT" \
    "select bucket, source, domain, count(distinct remote_ip) as crun_uniq_ips from $DATASET.summary_export where bucket like '%crun%' and (source='S3' or source='GS') group by bucket, source, domain order by crun_uniq_ips desc"

bq -q query \
    --use_legacy_sql=false \
    --format "$FORMAT" \
    --max_rows 10000 \
    "SELECT case when user_agent like '%toolkit%' then 'toolkit' when user_agent like '%cli%' then 'cli' else user_agent end as agent_type, sum(num_requests) as sessions FROM $DATASET.summary_export export group by agent_type order by sessions desc limit 50"

if [ "$STRIDES_SCOPE" == "private" ]; then
    bq -q query \
        --use_legacy_sql=false \
        --format "$FORMAT" \
        "SELECT bucket, source, domain, country_code, count(*) as sessions, count(distinct accession) as num_accesions FROM strides_analytics.summary_export_ca_masked group by bucket, source, domain, country_code order by count(*) desc"

    bq -q query \
        --use_legacy_sql=false \
        --format "$FORMAT" \
        --max_rows 10000 \
        "select day as day, sum(s3_requests) as s3_requests, sum(gs_requests) as gs_requests, sum(op_requests) as op_requests, internal from ( SELECT datetime_trunc(start_ts, day) as day, case when source='S3' then num_requests else 0 end as s3_requests, case when source='GS' then num_requests else 0 end as gs_requests, case when source='OP' then num_requests else 0 end as op_requests, domain like '%nih.gov%' as internal from strides_analytics.summary_export_ca_masked where (http_operations like '%GET%' or http_operations like '%HEAD%' ) and start_ts > '2021-01-01' ) group by internal, day order by internal, day limit 50"

else

    bq -q query \
        --use_legacy_sql=false \
        --format "$FORMAT" \
        "SELECT CASE WHEN key LIKE '%.cram%' THEN 'cram,crai' WHEN key LIKE '%.crai%' THEN 'cram,crai' WHEN key LIKE '%.bam%' THEN 'bam,bai' WHEN key LIKE '%.bai%' THEN 'bam,bai' WHEN key like '%.fastq.gz%' THEN 'fastq.gz' WHEN key like '%.fq.gz%' THEN 'fq.gz' WHEN key like '%.fastq.%' THEN 'fastq' WHEN key like '%.sam%' THEN 'sam' ELSE 'other' END AS type, format('%\'d',COUNT(*)) AS cnt, format('%\'d', cast(AVG(size) as int64)) AS average_size FROM $DATASET.objects_uniq GROUP BY type order by type"

    bq -q query \
        --use_legacy_sql=false \
        --format "$FORMAT" \
        "SELECT distinct source || '/' || bucket as unlogged_bucket from $DATASET.objects where source || '/' || bucket not in (select distinct source || '/' || regexp_extract(bucket,r'^[\S]+') from $DATASET.summary_export) order by unlogged_bucket"

    bq -q query \
        --use_legacy_sql=false \
        --format "$FORMAT" \
        "SELECT distinct source || '/' || regexp_extract(bucket,r'^[\S]+') as unlisted_bucket from $DATASET.summary_export where source || '/' || regexp_extract(bucket,r'^[\S]+') not in (select distinct source || '/' || bucket from $DATASET.objects) order by unlisted_bucket"

fi # public

for SOURCE in GS S3 OP; do
    bq -q query \
        --use_legacy_sql=false \
        --format "$FORMAT" \
        "select datetime_trunc(start_ts,day) as day, source, count(distinct remote_ip) as num_ips, count(distinct accession) as num_accs from $DATASET.summary_export where source='$SOURCE' group by day, source order by day desc limit 30"
done

bq -q query \
    --use_legacy_sql=false \
    --format "$FORMAT" \
    "select datetime_trunc(start_ts, month) as month, source, count(distinct remote_ip) as num_ips, count(distinct accession) as num_accs, sum(bytes_sent) as bytes_sent from $DATASET.summary_export where start_ts > date_sub(current_date(), interval 6 month) group by month, source order by source, month desc limit 30"

echo "End of BigQuery Report"
date
