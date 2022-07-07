#!/usr/bin/bash

# shellcheck source=strides_env.sh
. ./strides_env.sh

export CLOUDSDK_CORE_PROJECT="ncbi-logmon"
gcloud config set account 253716305623-compute@developer.gserviceaccount.com

skipload=false
if [ "$#" -eq 1 ]; then
    if [ "$1" = "skipload" ]; then
        skipload=true
    fi
fi

if [ "$skipload" = false ]; then
    gsutil du -s -h "$PARSE_BUCKET/logs_gs_$STRIDES_SCOPE/"
    gsutil du -s -h "$PARSE_BUCKET/logs_op_$STRIDES_SCOPE/"
    gsutil du -s -h "$PARSE_BUCKET/logs_s3_$STRIDES_SCOPE/"
    gsutil du -s -h "$MIRROR_BUCKET/gs_$STRIDES_SCOPE"
    gsutil du -s -h "$MIRROR_BUCKET/op_$STRIDES_SCOPE"
    gsutil du -s -h "$MIRROR_BUCKET/s3_$STRIDES_SCOPE"
    gsutil du -s -h gs://logmon_objects/gs
    gsutil du -s -h gs://logmon_objects/s3

# TODO: Partition/cluster large tables for incremental inserts and retrievals
# TODO: Materialized views that automatically refresh

echo " #### gs_parsed"
cat << EOF > gs_schema.json
    { "schema": { "fields": [
    { "name" : "accepted", "type": "BOOLEAN" },
    { "name" : "accession", "type": "STRING" },
    { "name" : "agent", "type": "STRING" },
    { "name" : "bucket", "type": "STRING" },
    { "name" : "extension", "type": "STRING" },
    { "name" : "filename", "type": "STRING" },
    { "name" : "host", "type": "STRING" },
    { "name" : "ip", "type": "STRING" },
    { "name" : "ip_region", "type": "STRING" },
    { "name" : "ip_type", "type": "INTEGER" },
    { "name" : "method", "type": "STRING" },
    { "name" : "operation", "type": "STRING" },
    { "name" : "path", "type": "STRING" },
    { "name" : "referer", "type": "STRING" },
    { "name" : "request_bytes", "type": "INTEGER" },
    { "name" : "request_id", "type": "STRING" },
    { "name" : "result_bytes", "type": "INTEGER" },
    { "name" : "source", "type": "STRING" },
    { "name" : "status", "type": "INTEGER" },
    { "name" : "time", "type": "INTEGER" },
    { "name" : "time_taken", "type": "INTEGER" },
    { "name" : "uri", "type": "STRING" },
    { "name" : "vers", "type": "STRING" } ,
    { "name" : "vdb_libc", "type": "STRING" },
    { "name" : "vdb_os", "type": "STRING" },
    { "name" : "vdb_phid_compute_env", "type": "STRING" },
    { "name" : "vdb_phid_guid", "type": "STRING" },
    { "name" : "vdb_phid_session_id", "type": "STRING" },
    { "name" : "vdb_release", "type": "STRING" },
    { "name" : "vdb_tool", "type": "STRING" }
    ]
  },
  "sourceFormat": "NEWLINE_DELIMITED_JSON",
  "sourceUris": [ "$PARSE_BUCKET/logs_gs_${STRIDES_SCOPE}${PARSE_VER}/recognized.*" ]
}
EOF

    jq -S -c -e . < gs_schema.json > /dev/null
    jq -S -c .schema.fields < gs_schema.json > gs_schema_only.json

    #bq mk --external_table_definition=gs_schema_only.json $DATASET.gs_parsed

    gsutil ls -lR "$PARSE_BUCKET/logs_gs_${STRIDES_SCOPE}${PARSE_VER}/recognized.*" | tail

    bq rm -f "$DATASET.gs_parsed" || true
    bq load \
        --source_format=NEWLINE_DELIMITED_JSON \
        "$DATASET.gs_parsed" \
        "$PARSE_BUCKET/logs_gs_${STRIDES_SCOPE}${PARSE_VER}/recognized.*" \
        gs_schema_only.json
    bq show --schema "$DATASET.gs_parsed"


echo " #### s3_parsed"
    cat << EOF > s3_schema.json
    { "schema": { "fields": [
        { "name" : "accepted", "type": "BOOLEAN" },
        { "name" : "accession", "type": "STRING" },
        { "name" : "agent", "type": "STRING" },
        { "name" : "auth_type", "type": "STRING" },
        { "name" : "bucket", "type": "STRING" },
        { "name" : "sig_ver", "type": "STRING" },
        { "name" : "cipher_suite", "type": "STRING" },
        { "name" : "error", "type": "STRING" },
        { "name" : "extension", "type": "STRING" },
        { "name" : "filename", "type": "STRING" },
        { "name" : "host_header", "type": "STRING" },
        { "name" : "host_id", "type": "STRING" },
        { "name" : "ip", "type": "STRING" },
        { "name" : "key", "type": "STRING" },
        { "name" : "method", "type": "STRING" },
        { "name" : "obj_size", "type": "STRING" },
        { "name" : "operation", "type": "STRING" },
        { "name" : "owner", "type": "STRING" },
        { "name" : "path", "type": "STRING" },
        { "name" : "referer", "type": "STRING" },
        { "name" : "request_id", "type": "STRING" },
        { "name" : "requester", "type": "STRING" },
        { "name" : "res_code", "type": "STRING" },
        { "name" : "res_len", "type": "STRING" },
        { "name" : "source", "type": "STRING" },
        { "name" : "time", "type": "STRING" },
        { "name" : "tls_version", "type": "STRING" },
        { "name" : "total_time", "type": "STRING" },
        { "name" : "turnaround_time", "type": "STRING" },
        { "name" : "vers", "type": "STRING" },
        { "name" : "version_id", "type": "STRING" },
        { "name" : "vdb_libc", "type": "STRING" },
        { "name" : "vdb_os", "type": "STRING" },
        { "name" : "vdb_phid_compute_env", "type": "STRING" },
        { "name" : "vdb_phid_guid", "type": "STRING" },
        { "name" : "vdb_phid_session_id", "type": "STRING" },
        { "name" : "vdb_release", "type": "STRING" },
        { "name" : "vdb_tool", "type": "STRING" },
        { "name" : "_extra", "type": "STRING" }
        ]
    },
    "sourceFormat": "NEWLINE_DELIMITED_JSON",
    "sourceUris": [ "$PARSE_BUCKET/logs_s3_${STRIDES_SCOPE}${PARSE_VER}/recogn*" ]
    }
EOF
    jq -S -c -e . < s3_schema.json > /dev/null
    jq -S -c .schema.fields < s3_schema.json > s3_schema_only.json

    gsutil ls -lR "$PARSE_BUCKET/logs_s3_${STRIDES_SCOPE}${PARSE_VER}/recognized.*" | tail

    bq rm -f "$DATASET.s3_parsed" || true
    bq load \
        --max_bad_records 5000 \
        --source_format=NEWLINE_DELIMITED_JSON \
        "$DATASET.s3_parsed" \
        "$PARSE_BUCKET/logs_s3_${STRIDES_SCOPE}${PARSE_VER}/recognized.*" \
        s3_schema_only.json

    bq show --schema "$DATASET.s3_parsed"

echo " #### op_parsed"
# TODO
    cat << EOF > op_schema.json
    { "schema": { "fields": [
        { "name" : "accepted", "type": "BOOLEAN" },
        { "name" : "accession", "type": "STRING" },
        { "name" : "agent", "type": "STRING" },
        { "name" : "extension", "type": "STRING" },
        { "name" : "filename", "type": "STRING" },
        { "name" : "forwarded", "type": "STRING" },
        { "name" : "ip", "type": "STRING" },
        { "name" : "method", "type": "STRING" },
        { "name" : "path", "type": "STRING" },
        { "name" : "port", "type": "INTEGER" },
        { "name" : "referer", "type": "STRING" },
        { "name" : "req_len", "type": "INTEGER" },
        { "name" : "req_time", "type": "STRING" },
        { "name" : "res_code", "type": "INTEGER" },
        { "name" : "res_len", "type": "INTEGER" },
        { "name" : "server", "type": "STRING" },
        { "name" : "source", "type": "STRING" },
        { "name" : "time", "type": "STRING" },
        { "name" : "user", "type": "STRING" },
        { "name" : "vers", "type": "STRING" },
        { "name" : "vdb_libc", "type": "STRING" },
        { "name" : "vdb_os", "type": "STRING" },
        { "name" : "vdb_phid_compute_env", "type": "STRING" },
        { "name" : "vdb_phid_guid", "type": "STRING" },
        { "name" : "vdb_phid_session_id", "type": "STRING" },
        { "name" : "vdb_release", "type": "STRING" },
        { "name" : "vdb_tool", "type": "STRING" }
        ]
    },
    "sourceFormat": "NEWLINE_DELIMITED_JSON",
    "sourceUris": [ "$PARSE_BUCKET/logs_op_${STRIDES_SCOPE}${PARSE_VER}/recogn*" ]
    }
EOF

    jq -S -c -e . < op_schema.json > /dev/null
    jq -S -c .schema.fields < op_schema.json > op_schema_only.json

    gsutil ls -lR "$PARSE_BUCKET/logs_op_${STRIDES_SCOPE}${PARSE_VER}/recognized.*" | tail

    bq rm -f "$DATASET.op_parsed" || true
#        "$PARSE_BUCKET/logs_op_public/recognized.*" \
#        "gs://logmon_logs_parsed_us/logs_op_public/recognized.*" \
    bq load \
        --max_bad_records 200000 \
        --source_format=NEWLINE_DELIMITED_JSON \
        "$DATASET.op_parsed" \
        "$PARSE_BUCKET/logs_op_${STRIDES_SCOPE}${PARSE_VER}/recognized.*" \
        op_schema_only.json

    bq show --schema "$DATASET.op_parsed"



echo " #### Parsed results"
bq -q query \
    --use_legacy_sql=false \
    "select source, accepted, min(time) as min_time, max(time) as max_time, count(*) as parsed_count from (select source, accepted, cast(time as string) as time from $DATASET.gs_parsed union all select source, accepted, cast(time as string) as time from $DATASET.s3_parsed union all select source, accepted, time as time from $DATASET.op_parsed) group by source, accepted order by source"

fi # skipload

echo " #### UDFs"
    QUERY=$(cat <<-ENDOFQUERY
CREATE OR REPLACE FUNCTION $DATASET.expand_bucket (bucket STRING, path STRING)
RETURNS STRING
AS
 (case
        WHEN regexp_contains(bucket, r'-ca-run-') THEN bucket || ' (Controlled Access ETL + BQS)'
        WHEN regexp_contains(bucket, r'-ca-crun-') THEN bucket || ' (Controlled Access ETL + BQS Cold)'
        WHEN regexp_contains(bucket, r'-ca-zq-') THEN bucket || ' (Controlled Access ETL - BQS)'
        WHEN regexp_contains(bucket, r'-ca-src-1') THEN bucket || ' (Controlled Access Original)'
        WHEN regexp_contains(bucket, r'-ca-src-[2-9]') THEN bucket || ' (Controlled Access Original Cold)'
        WHEN regexp_contains(bucket, r'-ca-') THEN bucket || ' (Controlled Access)'
        WHEN ends_with(bucket, '-us-east-1') THEN bucket || '  (Athena metadata)'
        WHEN ends_with(bucket, '-cov2') and
            regexp_contains(path, r'sra-src') THEN bucket || ' (Original)'
        WHEN ends_with(bucket, '-cov2') and
            regexp_contains(path, r'run') THEN bucket || ' (ETL + BQS)'
        WHEN regexp_contains(bucket, r'sra-pub-assembly-1') THEN bucket || ' (ETL - BQS)'
        WHEN regexp_contains(bucket, r'-zq-') THEN bucket || ' (ETL - BQS)'
        WHEN regexp_contains(bucket, r'-pub-crun-') THEN bucket || ' (ETL + BQS Cold)'
        WHEN regexp_contains(bucket, r'-run-') THEN bucket || ' (ETL + BQS)'
        WHEN regexp_contains(bucket, r'sra-pub-run-odp') THEN bucket || ' (ETL + BQS)'
        WHEN ends_with(bucket, 'sra-pub-src-1') THEN bucket || ' (Original)'
        WHEN ends_with(bucket, 'sra-pub-src-2') THEN bucket || ' (Original)'
        WHEN regexp_contains(bucket, r'sra-pub-src-') THEN bucket || ' (Original Cold)'
    ELSE bucket || ' (Unknown)'
    END)
ENDOFQUERY
)
    QUERY="${QUERY//\\/}"
    bq query --use_legacy_sql=false --batch=true "$QUERY"

# See LOGMON-93
    QUERY=$(cat <<-ENDOFQUERY
    CREATE OR REPLACE FUNCTION $DATASET.map_extension (extension STRING)
    RETURNS STRING
    AS
    (case
    WHEN regexp_contains(extension, r'\.bam') THEN 'BAM'
    WHEN regexp_contains(extension, r'\.cram') THEN 'CRAM'
    WHEN regexp_contains(extension, r'\.fastq') THEN 'FASTQ'
    WHEN regexp_contains(extension, r'\.fq') THEN 'FASTQ'
    WHEN regexp_contains(extension, r'\.fasta') THEN 'FASTA'
    WHEN regexp_contains(extension, r'\.fq') THEN 'FASTA'
    WHEN regexp_contains(extension, r'\.hdf5') THEN 'HDF5-BAX'
    WHEN regexp_contains(extension, r'\.bax\.h5') THEN 'HDF5-BAX'
    WHEN regexp_contains(extension, r'\.hdf5')  THEN 'HDF5-BAS'
    WHEN regexp_contains(extension, r'\.bas.h5') THEN 'HDF5-BAS'
    WHEN regexp_contains(extension, r'\.zq') THEN 'ZQ'
    WHEN regexp_contains(extension , r'/traces/refseq') THEN 'RefSeq'
    WHEN regexp_contains(extension , r'/traces%2Frefseq') THEN 'RefSeq'
    WHEN regexp_contains(extension , r'/sra-pub-refseq') THEN 'RefSeq'
    WHEN regexp_contains(extension , r'/traces/wgs') THEN 'WGS'
    WHEN regexp_contains(extension , r'/traces%2Fwgs') THEN 'WGS'
    WHEN regexp_contains(extension , r'/sragap') THEN 'dbGaP'
    WHEN regexp_contains(extension , r'/kmer') THEN 'kMer'
    WHEN regexp_contains(extension, '[DES]R[RZ]{6,10}') THEN 'SRA ETL'
    WHEN regexp_contains(extension, r'\.tar.gz') THEN 'TARGZ'
    WHEN regexp_contains(extension, r'\.tar') THEN 'TAR'
    WHEN regexp_contains(extension, r'.gff3') THEN 'GFF'
    WHEN regexp_contains(extension, r'BLAST_DB/') THEN 'blast_db'
    WHEN regexp_contains(extension, r'blast_db/') THEN 'blast_db'
    WHEN regexp_contains(extension, r'RAO/') THEN 'rao'
    WHEN regexp_contains(extension, r'rao/') THEN 'rao'
    WHEN regexp_contains(extension, r'SPDI/') THEN 'spdi'
    WHEN regexp_contains(extension, r'spdi/') THEN 'spdi'
    WHEN regexp_contains(extension, r'VCF/') THEN 'vcf'
    WHEN regexp_contains(extension, r'vcf/') THEN 'vcf'
    WHEN regexp_contains(extension, r'\.VCF') THEN 'vcf'
    WHEN regexp_contains(extension, r'\.vcf') THEN 'vcf'
    WHEN regexp_contains(extension, r'[DES]R[RZ][0-9]{6,10}') THEN 'SRA ETL'
    ELSE 'other'
    END)
ENDOFQUERY
)
    QUERY="${QUERY//\\/}"
    bq query --use_legacy_sql=false --batch=true "$QUERY"


echo " #### gs_fixed"
    QUERY=$(cat <<-ENDOFQUERY
    SELECT
    ip as remote_ip,
        parse_datetime('%s', cast ( cast (time/1000000 as int64) as string) ) as start_ts,
    datetime_add(
        parse_datetime('%s', cast ( cast (time/1000000 as int64) as string) ),
        interval time_taken microsecond) as end_ts,
    replace(agent, '-head', '') as user_agent,
    cast (status as string) as http_status,
    host as host,
    method as http_operation,
    path as request_uri,
    $DATASET.map_extension(path) as extension,
    referer as referer,
    regexp_extract(path,r'[DES]R[RZ][0-9]{5,10}') as accession,
    result_bytes as bytes_sent,
    substr(regexp_extract(path,r'[0-9]\.[0-9]{1,2}'),3) as version,
    $DATASET.expand_bucket(bucket, path) as bucket,
    ifnull(source,'GS') as source,
    current_datetime() as fixed_time
    FROM \\\`ncbi-logmon.$DATASET.gs_parsed\\\`
    WHERE ifnull(accepted,true)=true
ENDOFQUERY
)

    # TODO Hack, cause I can't understand bash backtick quoting
    QUERY="${QUERY//\\/}"
    bq rm --project_id ncbi-logmon -f "$DATASET.gs_fixed" || true
    # shellcheck disable=SC2016
    bq query \
    --project_id ncbi-logmon \
    --destination_table "$DATASET.gs_fixed" \
    --use_legacy_sql=false \
    --batch=true \
    --max_rows=5 \
    "$QUERY"

    bq show --schema "$DATASET.gs_fixed"

    #parse_datetime('%d.%m.%Y:%H:%M:%S 0', time) as start_ts,
    #[17/May/2019:23:19:24 +0000]
echo " #### s3_fixed"
    # LOGMON-1: Remove multiple -heads from agent
    QUERY=$(cat <<-ENDOFQUERY
    SELECT
    ip as remote_ip,
    parse_datetime('[%d/%b/%Y:%H:%M:%S +0000]', time) as start_ts,
    datetime_add(parse_datetime('[%d/%b/%Y:%H:%M:%S +0000]', time),
        interval cast (
                case when total_time='' THEN '0' ELSE total_time END
        as int64) millisecond) as end_ts,
    regexp_extract(path,r'[DES]R[RZ][0-9]{5,10}') as accession,
    substr(regexp_extract(path,r'[0-9]\.[0-9]{1,2}'),3) as version,
    case
        WHEN regexp_contains(agent, r'-head') THEN 'HEAD'
        ELSE method
        END as http_operation,
    cast(res_code as string) as http_status,
    host_header as host,
    cast (case WHEN res_len='' THEN '0' ELSE res_len END as int64) as bytes_sent,
    path as request_uri,
    $DATASET.map_extension(path) as extension,
    referer as referer,
    replace(agent, '-head', '') as user_agent,
    ifnull(source,'S3') as source,
    $DATASET.expand_bucket(bucket, path) as bucket,
    current_datetime() as fixed_time
    FROM \\\`ncbi-logmon.$DATASET.s3_parsed\\\`
    WHERE ifnull(accepted,true)=true
ENDOFQUERY
    )

    QUERY="${QUERY//\\/}"
    bq rm --project_id ncbi-logmon -f "$DATASET.s3_fixed" || true
    # shellcheck disable=SC2016
    bq query \
    --project_id ncbi-logmon \
    --destination_table "$DATASET.s3_fixed" \
    --use_legacy_sql=false \
    --batch=true \
    --max_rows=5 \
    "$QUERY"

    bq show --schema "$DATASET.s3_fixed"

echo " #### op_fixed1"
    # LOGMON-1: Remove multiple -heads from agent
    QUERY=$(cat <<-ENDOFQUERY
    SELECT
    ip as remote_ip,
    case
        WHEN ends_with(time, '-0400]') THEN
            safe.parse_datetime('[%d/%b/%Y:%H:%M:%S -0400]', time)
        ELSE
            safe.parse_datetime('[%d/%b/%Y:%H:%M:%S -0500]', time)
        END as start_ts,
    case
        WHEN ends_with(time, '-0400]') THEN
            datetime_add(
                safe.parse_datetime('[%d/%b/%Y:%H:%M:%S -0400]', time),
                interval cast(1000*cast (req_time as float64) as int64)
                millisecond)
        ELSE
            datetime_add(
                safe.parse_datetime('[%d/%b/%Y:%H:%M:%S -0500]', time),
                interval cast(1000*cast (req_time as float64) as int64)
                millisecond)
        END as end_ts,
    accession,
    substr(regexp_extract(path,r'[0-9]\.[0-9]{1,2}'),3) as version,
    case
        WHEN regexp_contains(agent, r'-head') THEN 'HEAD'
        ELSE method
        END as http_operation,
    cast(res_code as string) as http_status,
    server as host,
    res_len as bytes_sent,
    path as request_uri,
    referer as referer,
    replace(agent, '-head', '') as user_agent
    FROM \\\`ncbi-logmon.$DATASET.op_parsed\\\`
    WHERE ifnull(accepted,true)=true
ENDOFQUERY
    )

    QUERY="${QUERY//\\/}"
    bq rm --project_id ncbi-logmon -f "$DATASET.op_fixed1" || true
    # shellcheck disable=SC2016

    bq query \
    --project_id ncbi-logmon \
    --destination_table "$DATASET.op_fixed1" \
    --time_partitioning_field=start_ts \
    --use_legacy_sql=false \
    --batch=true \
    --max_rows=5 \
    "$QUERY"

    bq show --schema "$DATASET.op_fixed1"

echo " #### op_fixed"
    QUERY=$(cat <<-ENDOFQUERY
    SELECT
    remote_ip,
    start_ts,
    end_ts,
    accession,
    version,
    http_operation,
    http_status,
    host,
    bytes_sent,
    request_uri,
    $DATASET.map_extension(request_uri) as extension,
    referer,
    'OP' as source,
    user_agent,
    current_datetime() as fixed_time
    FROM \\\`ncbi-logmon.$DATASET.op_fixed1\\\`
ENDOFQUERY
    )

    QUERY="${QUERY//\\/}"
    bq rm --project_id ncbi-logmon -f "$DATASET.op_fixed" || true
    # shellcheck disable=SC2016

    bq query \
    --project_id ncbi-logmon \
    --destination_table "$DATASET.op_fixed" \
    --time_partitioning_field=start_ts \
    --use_legacy_sql=false \
    --batch=true \
    --max_rows=5 \
    "$QUERY"

    bq show --schema "$DATASET.op_fixed"


echo " ##### detail_export"
echo " ##### detail_export_gs"
    QUERY=$(cat <<-ENDOFQUERY
SELECT
    accession as accession,
    bucket as bucket,
    bytes_sent as bytes_sent,
    current_datetime() as export_time,
    end_ts,
    fixed_time as fixed_time,
    host as host,
    http_status as http_status,
    remote_ip as remote_ip,
    http_operation as http_operation,
    request_uri as request_uri,
    extension as extension,
    referer as referer,
    source as source,
    start_ts,
    user_agent as user_agent,
    version as version,
    case
        WHEN http_operation in ('GET', 'HEAD') THEN 0
        WHEN http_operation='POST' THEN 1
        WHEN http_operation='PUT' THEN 2
        WHEN http_operation='DELETE' THEN 3
        WHEN http_operation='PATCH' THEN 4
        WHEN http_operation='OPTIONS' THEN 5
        WHEN http_operation='TRACE' THEN 6
        WHEN http_operation='CONNECT' THEN 7
    ELSE 99
    END as http_operation_combined
    FROM \\\`$DATASET.gs_fixed\\\`
ENDOFQUERY
    )
QUERY="${QUERY//\\/}"

bq rm --project_id ncbi-logmon -f "$DATASET.detail_export_gs" || true
    # shellcheck disable=SC2016
bq query \
    --project_id ncbi-logmon \
    --destination_table "$DATASET.detail_export_gs" \
    --use_legacy_sql=false \
    --batch=true \
    --max_rows=5 \
    --time_partitioning_field=start_ts \
    "$QUERY"


echo " ##### detail_export_s3"
    QUERY=$(cat <<-ENDOFQUERY
SELECT
    accession as accession,
    bucket as bucket,
    bytes_sent as bytes_sent,
    current_datetime() as export_time,
    end_ts,
    fixed_time as fixed_time,
    host as host,
    http_status as http_status,
    remote_ip as remote_ip,
    http_operation as http_operation,
    request_uri as request_uri,
    extension as extension,
    referer as referer,
    source as source,
    start_ts,
    user_agent as user_agent,
    version as version,
    case
        WHEN http_operation in ('GET', 'HEAD') THEN 0
        WHEN http_operation='POST' THEN 1
        WHEN http_operation='PUT' THEN 2
        WHEN http_operation='DELETE' THEN 3
        WHEN http_operation='PATCH' THEN 4
        WHEN http_operation='OPTIONS' THEN 5
        WHEN http_operation='TRACE' THEN 6
        WHEN http_operation='CONNECT' THEN 7
    ELSE 99
    END as http_operation_combined
    FROM \\\`$DATASET.s3_fixed\\\`
ENDOFQUERY
    )
QUERY="${QUERY//\\/}"

bq rm --project_id ncbi-logmon -f "$DATASET.detail_export_s3" || true
    # shellcheck disable=SC2016
bq query \
    --project_id ncbi-logmon \
    --destination_table "$DATASET.detail_export_s3" \
    --use_legacy_sql=false \
    --batch=true \
    --max_rows=5 \
    --time_partitioning_field=start_ts \
    "$QUERY"

echo " ##### detail_export_op"
    QUERY=$(cat <<-ENDOFQUERY
SELECT
    accession as accession,
    host as bucket,
    bytes_sent as bytes_sent,
    current_datetime() as export_time,
    end_ts,
    fixed_time as fixed_time,
    host as host,
    http_status as http_status,
    remote_ip as remote_ip,
    http_operation as http_operation,
    request_uri as request_uri,
    extension as extension,
    referer as referer,
    source as source,
    start_ts,
    user_agent as user_agent,
    version as version,
    case
        WHEN http_operation in ('GET', 'HEAD') THEN 0
        WHEN http_operation='POST' THEN 1
        WHEN http_operation='PUT' THEN 2
        WHEN http_operation='DELETE' THEN 3
        WHEN http_operation='PATCH' THEN 4
        WHEN http_operation='OPTIONS' THEN 5
        WHEN http_operation='TRACE' THEN 6
        WHEN http_operation='CONNECT' THEN 7
    ELSE 99
    END as http_operation_combined
    FROM \\\`$DATASET.op_fixed\\\`
ENDOFQUERY
    )
QUERY="${QUERY//\\/}"

bq rm --project_id ncbi-logmon -f "$DATASET.detail_export_op" || true
    # shellcheck disable=SC2016
bq query \
    --project_id ncbi-logmon \
    --destination_table "$DATASET.detail_export_op" \
    --use_legacy_sql=false \
    --batch=true \
    --max_rows=5 \
    --time_partitioning_field=start_ts \
    "$QUERY"

echo " ##### detail_export_union"
    QUERY=$(cat <<-ENDOFQUERY
SELECT * FROM \\\`$DATASET.detail_export_gs\\\`
UNION ALL
SELECT * FROM \\\`$DATASET.detail_export_op\\\`
UNION ALL
SELECT * FROM \\\`$DATASET.detail_export_s3\\\`
ENDOFQUERY
    )
    QUERY="${QUERY//\\/}"

    bq rm --project_id ncbi-logmon -f "$DATASET.detail_export" || true
    # shellcheck disable=SC2016
    bq query \
    --project_id ncbi-logmon \
    --destination_table "$DATASET.detail_export" \
    --use_legacy_sql=false \
    --batch=true \
    --max_rows=5 \
    --time_partitioning_field=start_ts \
    "$QUERY"

    bq show --schema "$DATASET.detail_export"

echo " ##### detail_export prune"
bq -q query \
    --use_legacy_sql=false \
    "delete from $DATASET.detail_export where start_ts < '2000-01-01'"


#if [ "$STRIDES_SCOPE" == "public" ]; then
#    echo " ###  improve file_exts
#        QUERY=$(cat <<-ENDOFQUERY
#        update $DATASET.detail_export
#        set
#        extension=
#            case
#                when request_uri like '%BLAST_DB/%' then 'blast_db'
#                when request_uri like '%RAO/%' then 'rao'
#                when request_uri like '%SPDI/%' then 'spdi'
#                when request_uri like '%VCF/%' then 'vcf'
#            else request_uri
#            end
#    where request_uri like '%cov2%'
#
#ENDOFQUERY
#    )
#        QUERY="${QUERY//\\/}"
#
#        bq query --use_legacy_sql=false --batch=true "$QUERY"
#fi

echo " ###  summary_grouped"
    QUERY=$(cat <<-ENDOFQUERY
    SELECT
        accession,
        user_agent,
        remote_ip,
        host,
        bucket,
        source,
        count(*) as num_requests,
        min(start_ts) as start_ts,
        max(end_ts) as end_ts,
        string_agg(distinct http_operation order by http_operation) as http_operations,
        string_agg(distinct http_status order by http_status) as http_statuses,
        string_agg(distinct referer) as referers,
        string_agg(distinct extension order by extension) as file_exts,
        sum(bytes_sent) as bytes_sent,
        current_datetime() as export_time
    FROM
        \\\`$DATASET.detail_export\\\`
    GROUP BY
        datetime_trunc(start_ts, day), accession, user_agent,
        remote_ip, host, bucket, source, http_operation_combined
    HAVING
        bytes_sent > 0
ENDOFQUERY
    )

    QUERY="${QUERY//\\/}"

    bq rm --project_id ncbi-logmon -f "$DATASET.summary_grouped" || true
    # shellcheck disable=SC2016
    bq query \
    --destination_table "$DATASET.summary_grouped" \
    --use_legacy_sql=false \
    --batch=true \
    --max_rows=5 \
    "$QUERY"

    bq show --schema "$DATASET.summary_grouped"


echo " ###  op_sess"
if [ "$STRIDES_SCOPE" == "public" ]; then
    # LOGMON-85: op_fixed is 4/21->, excluding 4/24, 6/25, 6/260
        QUERY=$(cat <<-ENDOFQUERY
        INSERT INTO $DATASET.summary_grouped
        (accession, user_agent, remote_ip, host,
        bucket, source, num_requests, start_ts, end_ts,
        http_operations,
        http_statuses,
        referers, bytes_sent, export_time, file_exts)
        SELECT
            acc,
            substr(agent,0,200),
            ip, domain,
            domain, 'OP', cnt, cast (start as datetime), cast (\\\`end\\\` as datetime),
            replace(cmds,' ',','),
            replace(status,' ',','),
            '', bytecount, current_datetime(),
            $DATASET.map_extension(acc)
        FROM \\\`ncbi-logmon.$DATASET.op_sess\\\`
        WHERE regexp_contains(acc,r'[DES]R[RZ][0-9]{5,10}')
        AND start between '2016-10-01' and '2020-04-21'
        AND domain not like '%amazon%'
        AND domain not like '%gap%'
        AND domain not like '%gap-download%'

ENDOFQUERY
    )
else
        QUERY=$(cat <<-ENDOFQUERY
        INSERT INTO $DATASET.summary_grouped
        (accession, user_agent, remote_ip, host,
        bucket, source, num_requests, start_ts, end_ts,
        http_operations,
        http_statuses,
        referers, bytes_sent, export_time, file_exts)
        SELECT
            acc,
            substr(agent,0,200),
            ip, domain,
            domain, 'OP', cnt, cast (start as datetime), cast (\\\`end\\\` as datetime),
            replace(cmds,' ',','),
            replace(status,' ',','),
            '', bytecount, current_datetime(),
            $DATASET.map_extension(acc)
        FROM \\\`ncbi-logmon.strides_analytics.op_sess\\\`
        WHERE regexp_contains(acc,r'[DES]R[RZ][0-9]{5,10}')
        AND start between '2016-10-01' and '2020-04-21'
ENDOFQUERY
    )

fi # public
        QUERY="${QUERY//\\/}"

        bq query \
        --use_legacy_sql=false \
        --batch=true \
        "$QUERY"

    echo " ###  fix op_sess"
        QUERY=$(cat <<-ENDOFQUERY
        update $DATASET.summary_grouped
        set
        bucket=ifnull(bucket,'') || ' (ETL + BQS)',
        http_operations=replace(http_operations,' ',','),
        http_statuses=replace(http_statuses,' ',','),
        user_agent=replace(user_agent, '-head', '')
        where source='OP'

ENDOFQUERY
    )
        QUERY="${QUERY//\\/}"

        bq query --use_legacy_sql=false --batch=true "$QUERY"

echo " ###  uniq_ips"
    bq query --use_legacy_sql=false --batch=true "DROP TABLE IF EXISTS $DATASET.uniq_ips"

    QUERY=$(cat <<-ENDOFQUERY

    CREATE OR REPLACE TABLE $DATASET.uniq_ips
    PARTITION BY RANGE_BUCKET(ipint, GENERATE_ARRAY(0, 4294967296, 429496729))
    AS
    SELECT DISTINCT remote_ip as remote_ip,
        case when
            regexp_contains(
            remote_ip,
                r'^[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}$'
                )
            then
                net.ipv4_to_int64(net.safe_ip_from_string(remote_ip))
        else
            6
    end as ipint
    FROM (
    select remote_ip from \\\`ncbi-logmon.strides_analytics.summary_grouped\\\`
    union all
    select remote_ip from \\\`ncbi-logmon.strides_analytics_private.summary_grouped\\\`
        )
    WHERE length(remote_ip) < 100
ENDOFQUERY
)
    QUERY="${QUERY//\\/}"

    bq query --use_legacy_sql=false --batch=true "$QUERY"


RUN="yes"
if [ "$RUN" = "yes" ]; then
# Only needs to be run when a lot of new IP addresses appear
echo " ###  iplookup_part"
    QUERY=$(cat <<-ENDOFQUERY

    CREATE OR REPLACE TABLE $DATASET.ip2location_part
    PARTITION BY
       RANGE_BUCKET(ip_from, GENERATE_ARRAY(0, 4294967296, 429496729))
    AS
    SELECT * from \\\`ncbi-logmon.strides_analytics.ip2location\\\`
ENDOFQUERY
)
    #WHERE remote_ip like '%.%'
    QUERY="${QUERY//\\/}"

    bq query --use_legacy_sql=false --batch=true "$QUERY"

echo " ###  iplookup_new3"
    QUERY=$(cat <<-ENDOFQUERY
    CREATE OR REPLACE TABLE $DATASET.iplookup_new3
    (remote_ip string,
     ipint int64,
     country_code string,
     region_name string,
     city_name string)
ENDOFQUERY
)
    bq query --use_legacy_sql=false --batch=true "$QUERY"

   #step=429496729
    step=100000000
    for part in $(seq 0 $step 4294967296); do
        top=$((step+part))
        QUERY=$(cat <<-ENDOFQUERY
        INSERT INTO $DATASET.iplookup_new3
        SELECT remote_ip, ipint, country_code, region_name, city_name
        FROM \\\`ncbi-logmon.strides_analytics.ip2location_part\\\`
        JOIN \\\`ncbi-logmon.$DATASET.uniq_ips\\\`
        ON (ipint >= ip_from and ipint <= ip_to)
        WHERE ipint   between $part and $top AND
            ip_from between $part and $top
ENDOFQUERY
)

        QUERY="${QUERY//\\/}"
        #echo $QUERY

        echo "   ipint between $part and $top"
        bq query --use_legacy_sql=false --batch=true "$QUERY"
    done
fi # RUN

    echo " ### Find new IP addresses"
        QUERY=$(cat <<-ENDOFQUERY
    insert into ncbi-logmon.strides_analytics.rdns (ip)
    select distinct remote_ip as ip from $DATASET.summary_export where remote_ip not in (select distinct ip from ncbi-logmon.strides_analytics.rdns
    )
ENDOFQUERY
)
    QUERY="${QUERY//\\/}"
    bq query --use_legacy_sql=false --batch=true "$QUERY"

# if [ "$STRIDES_SCOPE" == "public" ]; then
echo " ### Find internal PUT/POST IPs"
    QUERY=$(cat <<-ENDOFQUERY
    UPDATE strides_analytics.rdns
    SET DOMAIN="NCBI Cloud (put.nlm.nih.gov)"
    WHERE ip in (
    SELECT distinct  remote_ip
    FROM \\\`ncbi-logmon.$DATASET.summary_export\\\`
    where http_operations like '%P%'
    and http_statuses like '%20%')
ENDOFQUERY
)
    QUERY="${QUERY//\\/}"
    bq query --use_legacy_sql=false --batch=true "$QUERY"

# https://confluence.ncbi.nlm.nih.gov/pages/viewpage.action?spaceKey=Edu&title=NCBI+Cloud+Education+Onboarding+Reference+Page
echo " ### Find internal IAMs IPs"
    QUERY=$(cat <<-ENDOFQUERY
    CREATE OR REPLACE TABLE strides_analytics.internal_rdns AS
    SELECT ip, string_agg(distinct requester) as reqagg
    FROM \\\`ncbi-logmon.$DATASET.s3_parsed\\\`
        WHERE
           requester like '%036922703339%'
        or requester like '%065193671012%'
        or requester like '%182221319577%'
        or requester like '%250813660784%'
        or requester like '%350784804014%'
        or requester like '%377855366243%'
        or requester like '%388320727108%'
        or requester like '%396554159273%'
        or requester like '%492028860168%'
        or requester like '%513880734999%'
        or requester like '%820248442686%'
        or requester like '%arn:aws:iam::018000097103%'
        or requester like '%arn:aws:iam::228184908524%'
        or requester like '%arn:aws:iam::313921231432%'
        or requester like '%arn:aws:iam::651740271041%'
        or requester like '%arn:aws:iam::783971887864%'
        or requester like '%arn:aws:iam::867126678632%'
        or requester like '%arn:aws:sts::184059545989%'
        or requester like '%arn:aws:sts::784757538848%'
        or requester like '%414262389673%'
        or requester like '%293555909480%'
        or requester like '%347258802972%'
        or requester like '%865472100019%'
        or requester like '%591266071882%'
    GROUP BY ip
ENDOFQUERY
    )
    QUERY="${QUERY//\\/}"
    bq query --use_legacy_sql=false --batch=true "$QUERY"


    QUERY=$(cat <<-ENDOFQUERY
    DELETE FROM strides_analytics.rdns
    WHERE ip in (select distinct ip from strides_analytics.internal_rdns)
ENDOFQUERY
)
    QUERY="${QUERY//\\/}"
    bq query --use_legacy_sql=false --batch=true "$QUERY"


    QUERY=$(cat <<-ENDOFQUERY
    INSERT INTO strides_analytics.rdns (ip, domain)
    SELECT ip, "NCBI Cloud (nlm.nih.gov " || reqagg || ")" as domain
    FROM strides_analytics.internal_rdns t2
ENDOFQUERY
)
    QUERY="${QUERY//\\/}"
    bq query --use_legacy_sql=false --batch=true "$QUERY"



echo " ### Update RDNS"
    QUERY=$(cat <<-ENDOFQUERY
    UPDATE strides_analytics.rdns
    SET DOMAIN="Unknown"
    WHERE domain is null
ENDOFQUERY
)
    QUERY="${QUERY//\\/}"
    bq query --use_legacy_sql=false --batch=true "$QUERY"

    QUERY=$(cat <<-ENDOFQUERY
    UPDATE strides_analytics.rdns
    SET DOMAIN="IPv6"
    WHERE ip like '%:%'
    and domain='Unknown'
ENDOFQUERY
)
    QUERY="${QUERY//\\/}"
    bq query --use_legacy_sql=false --batch=true "$QUERY"

    QUERY=$(cat <<-ENDOFQUERY
    UPDATE strides_analytics.rdns
    SET DOMAIN="IPv6 IANA Unicast (ncbi.nlm.nih.gov)"
    WHERE ip like 'fda3%'
ENDOFQUERY
)
    QUERY="${QUERY//\\/}"
    bq query --use_legacy_sql=false --batch=true "$QUERY"
# fi # public


echo " ###  summary_export"
    QUERY=$(cat <<-ENDOFQUERY
    SELECT
    accession,
    user_agent,
    grouped.remote_ip,
    host,
    bucket,
    grouped.source,
    num_requests,
    start_ts,
    end_ts,
    http_operations,
    http_statuses,
    referers,
    file_exts,
    bytes_sent,
    current_datetime() as export_time,
    case
        WHEN rdns.domain is null or rdns.domain='' or rdns.domain='Unknown' or rdns.domain='unknown'
            THEN 'Unknown (' || country_code || ')'
        ELSE rdns.domain
    END as domain,
    ifnull(region_name,'Unknown') as region_name,
    ifnull(country_code,'Unknown') as country_code,
    ifnull(city_name,'Unknown') as city_name,
    ifnull(organism, 'Unknown') as ScientificName,
    ifnull(consent,'Unknown') as consent,
    cast (mbytes as int64) as accession_size_mb
    FROM \\\`$DATASET.summary_grouped\\\` grouped
    LEFT JOIN \\\`strides_analytics.rdns\\\` rdns
        ON grouped.remote_ip=rdns.ip
    LEFT JOIN \\\`$DATASET.iplookup_new3\\\` iplookup
        ON grouped.remote_ip=iplookup.remote_ip
    LEFT JOIN \\\`nih-sra-datastore.sra.metadata\\\` metadata
        ON accession=metadata.acc
    WHERE
        accession IS NOT NULL AND accession != ""
ENDOFQUERY
    )

#    LEFT JOIN \\\`$DATASET.$STRIDES_SCOPE_fix\\\`
    QUERY="${QUERY//\\/}"

    bq cp -f "$DATASET.summary_export" "$DATASET.summary_export_$YESTERDAY"
    bq rm --project_id ncbi-logmon -f "$DATASET.summary_export" || true

    # shellcheck disable=SC2016
    bq query \
    --destination_table "$DATASET.summary_export" \
    --use_legacy_sql=false \
    --batch=true \
    --max_rows=5 \
    "$QUERY"
    bq show --schema "$DATASET.summary_export"

    OLD=$(date -d "-7 days" "+%Y%m%d")
    bq rm --project_id ncbi-logmon -f "$DATASET.summary_export_$OLD" || true

echo " ### fix summary_export for NIH"
    QUERY=$(cat <<-ENDOFQUERY
    UPDATE $DATASET.summary_export
    SET city_name='Bethesda',
    region_name='Maryland',
    country_code='US'
    WHERE domain like '%nih.gov%' and (city_name is null or city_name='Unknown')
ENDOFQUERY
)
    QUERY="${QUERY//\\/}"
    bq query --use_legacy_sql=false --batch=true "$QUERY"



echo " ###  export to GS"
    gsutil rm -f "gs://logmon_export/detail/detail.$DATE.*.json.gz" || true
#    bq extract \
#    --destination_format NEWLINE_DELIMITED_JSON \
#    --compression GZIP \
#    '$DATASET.detail_export' \
#    "gs://logmon_export/detail/detail.$DATE.*.json.gz"

    gsutil rm -f "gs://logmon_export/summary/summary.$DATE.$STRIDES_SCOPE*.json.gz" || true
    bq extract \
    --destination_format NEWLINE_DELIMITED_JSON \
    --compression GZIP \
    "$DATASET.summary_export" \
    "gs://logmon_export/summary/summary.$DATE.$STRIDES_SCOPE.*.json.gz"

    gsutil rm -f "gs://logmon_export/uniq_ips/uniq_ips.$DATE.$STRIDES_SCOPE.*.json.gz" || true
    bq extract \
    --destination_format NEWLINE_DELIMITED_JSON \
    --compression NONE \
    "$DATASET.uniq_ips" \
    "gs://logmon_export/uniq_ips/uniq_ips.$DATE.$STRIDES_SCOPE.json"



echo " ###  copy to filesystem"
#    mkdir -p "$PANFS/detail"
#    cd "$PANFS/detail" || exit
#    rm -f "$PANFS"/detail/detail."$DATE".* || true
#    gsutil cp -r "gs://logmon_export/detail/detail.$DATE.*" "$PANFS/detail/"

#    mkdir -p "$PANFS/summary"
#    cd "$PANFS/summary" || exit
#    rm -f "$PANFS"/summary/summary."$DATE".* || true
#    gsutil cp -r "gs://logmon_export/summary/summary.$DATE.*" "$PANFS/summary/"

    mkdir -p "$PANFS/uniq_ips"
    cd "$PANFS/uniq_ips" || exit
    rm -f "$PANFS/uniq_ips/uniq_ips.$DATE.$STRIDES_SCOPE".* || true
    gsutil cp -r "gs://logmon_export/uniq_ips/uniq_ips.$DATE.$STRIDES_SCOPE.*" "$PANFS/uniq_ips/"



if [ "$STRIDES_SCOPE" == "private" ]; then
    QUERY=$(cat <<-ENDOFQUERY
    DELETE from $DATASET.summary_export
    where source='OP' and
      ( host not like '%download%' or
        consent in ('public', 'none', 'Unknown') )
ENDOFQUERY
)
    QUERY="${QUERY//\\/}"
    bq query --use_legacy_sql=false --batch=true "$QUERY"


    QUERY=$(cat <<-ENDOFQUERY
    SELECT *
        REPLACE
        (
        "..." AS remote_ip,
        "..." AS city_name,
        CASE
            WHEN regexp_contains(domain, r'.nih.gov') THEN domain
            WHEN regexp_contains(domain, r'Unknown') then domain
            WHEN regexp_contains(domain, r'AWS') THEN domain
            WHEN regexp_contains(domain, r'GCP') THEN domain
            WHEN regexp_contains(domain, r'\.edu\.au') THEN '*.edu.au'
            WHEN regexp_contains(domain, r'\.edu') THEN '*.edu (' || country_code || ')'
            WHEN regexp_contains(domain, r'\.ed') THEN '*.edu ('  || country_code || ')'
            WHEN regexp_contains(domain, r'\.edu') THEN '*.edu (' || country_code || ')'
            WHEN regexp_contains(domain, r'\.gov') THEN '*.gov'
            WHEN regexp_contains(domain, r'ebi\.ac\.uk') THEN domain  || ' (' || country_code || ')'
            WHEN regexp_contains(domain, r'\.ac\.uk') THEN '*.ac.uk (' || country_code || ')'
            WHEN regexp_contains(domain, r'\.ac\.jp') THEN '*.ac.jp (' || country_code || ')'
            WHEN regexp_contains(domain, r'\.cn') THEN '*.cn ('  || country_code || ')'
            WHEN regexp_contains(domain, r'\.cn\.') THEN '*.cn (' || country_code || ')'
            WHEN regexp_contains(domain, r'hinamobile') THEN '*.cn (' || country_code || ')'
            WHEN regexp_contains(domain, r'\.com') THEN '*.com ('  || country_code || ')'
            ELSE '(' || country_code || ')'
         END AS domain,
         regexp_replace(user_agent, r'phid=[0-9a-z,=.]+', '...') as user_agent
        )
    from $DATASET.summary_export
    where consent not in
    ('DS-MHIV', 'DS-INF', 'DS-PTL', 'DS-MRUH')
ENDOFQUERY
)

# TODO: Use cloud_analytics.isConsentPublic()

#    QUERY="${QUERY//\\/}"

echo " ###  masking"
    bq cp -f "strides_analytics.summary_export_ca_masked" "strides_analytics.summary_export_ca_masked_$YESTERDAY" || true
    bq rm --project_id ncbi-logmon -f "strides_analytics.summary_export_ca_masked" || true

    # shellcheck disable=SC2016
    bq query \
    --project_id ncbi-logmon \
    --destination_table "strides_analytics.summary_export_ca_masked" \
    --use_legacy_sql=false \
    --batch=true \
    --max_rows=5 \
    "$QUERY"
else # not private
    echo "Cleanup large temp tables"
    bq rm --project_id ncbi-logmon -f "$DATASET.detail_export" || true
    bq rm --project_id ncbi-logmon -f "$DATASET.detail_export_gs" || true
    bq rm --project_id ncbi-logmon -f "$DATASET.detail_export_op" || true
    bq rm --project_id ncbi-logmon -f "$DATASET.detail_export_s3" || true

    bq rm --project_id ncbi-logmon -f "$DATASET.cloudian_fixed" || true
    bq rm --project_id ncbi-logmon -f "$DATASET.gs_fixed" || true
    bq rm --project_id ncbi-logmon -f "$DATASET.op_fixed" || true
    bq rm --project_id ncbi-logmon -f "$DATASET.op_fixed1" || true
    bq rm --project_id ncbi-logmon -f "$DATASET.s3_fixed" || true

    bq rm --project_id ncbi-logmon -f "$DATASET.gs_parsed" || true
    bq rm --project_id ncbi-logmon -f "$DATASET.op_parsed" || true
    bq rm --project_id ncbi-logmon -f "$DATASET.s3_parsed" || true
fi # private


echo "bigquery_export.sh complete"
date
