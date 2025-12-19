#!/bin/bash

# shellcheck source=strides_env.sh
. ./strides_env.sh

export GOOGLE_APPLICATION_CREDENTIALS=$HOME/sandbox-blast-847af7ab431a.json
gcloud config set account 1008590670571-compute@developer.gserviceaccount.com
export CLOUDSDK_CORE_PROJECT="ncbi-sandbox-blast"

export PATH="$PATH":/opt/sybase/utils/bin
mkdir -p "$VASTFS/sra_main"

cd "$VASTFS/sra_main" || exit

echo -e "SELECT [service_name] + '://' + bucket as full_bucket, [service_name] ,[bucket] ,[geo_region] ,[storage_class] ,[status] ,[created] ,[root_url] ,[sra_owned] ,[file_cnt] ,[protected] ,[open_data] ,[restricted] ,[mirrors] FROM [SRA_Main].[dbo].[SRAFilesDomain] where service_name in ('s3','gs') and bucket like 'sra-%' order by status desc, storage_class desc, service_name, bucket;\ngo\n" | \
    sqsh-ms-lb -m csv -h -S SRA_BATCH -U anyone -a 1 | \
    zstd -19 -c > "sra_buckets.$DATE.csv.zstd"

echo -e "select * from [SRA_Main].[dbo].[SRAFiles] order by acc,file_id;\ngo\n" |
    sqsh-ms-lb -m csv -h -S SRA_BATCH -U anyone -a 1 |
    zstd -19 -c > "sramain.$DATE.csv.zstd"

find "$VASTFS"/sra_main/ -type f -mtime +30 -delete
