#!/bin/bash

# shellcheck source=strides_env.sh
. ./strides_env.sh

export GOOGLE_APPLICATION_CREDENTIALS=$HOME/sandbox-blast-847af7ab431a.json
gcloud config set account 1008590670571-compute@developer.gserviceaccount.com
export CLOUDSDK_CORE_PROJECT="ncbi-sandbox-blast"

export PATH="$PATH":/opt/sybase/utils/bin
mkdir -p "$VASTFS/sra_main"

cd "$VASTFS/sra_main" || exit

echo -e "SELECT [service_name] + '://' + bucket as full_bucket, [service_name] ,[bucket] ,[geo_region] ,[storage_class] ,[status] ,[created] ,[root_url] ,[sra_owned] ,[file_cnt] ,[protected] ,[open_data] ,[restricted] ,[mirrors] FROM [SRA_Main].[dbo].[SRAFilesDomain] (nolock) where service_name in ('s3','gs') and bucket like 'sra-%' order by status desc, storage_class desc, service_name, bucket;\ngo\n" | \
    sqsh-ms-lb -m csv -h -S SRA_BATCH -U anyone -a 1 \
    > "sra_buckets.$DATE.csv"

echo -e "select * from [SRA_Main].[dbo].[SRAFiles] (nolock) order by acc,file_id;\ngo\n" |
    sqsh-ms-lb -m csv -h -S SRA_BATCH -U anyone -a 1 |
    zstd -19 -c > "sramain.$DATE.csv.zstd"


#echo -e "SELECT acc, create_date FROM [SRA_Main].[dbo].[SRAFiles] (nolock) where semantic_name='run.zq' and is_current=1 and file_id in ( select file_id FROM [SRA_Main].[dbo].[SRAFilesLocation]  (nolock) where domain_id in ( select domain_id FROM [SRA_Main].[dbo].[SRAFilesDomain] (nolock) where service_name='sra-sos') and availability='live') order by acc;\ngo\n" | \
#    sqsh-ms-lb -m csv -h -S SRA_BATCH -U anyone -a 1 | \
#    zstd -19 -c > "op_zq.$DATE.csv.zstd"

#echo -e "SELECT  acc, create_date, semantic_name, is_current FROM [SRA_Main].[dbo].[SRAFiles] (nolock) where is_current=1 and file_id in ( SELECT [file_id] FROM [SRA_Main].[dbo].[SRAFilesAnnotation] (nolock) where annot_type='delite') order by acc;\ngo\n" |\
#    sqsh-ms-lb -m csv -h -S SRA_BATCH -U anyone -a 1 | \
#    zstd -19 -c > "op_zq_annot.$DATE.csv.zstd"

#echo -e "SELECT sf.file_id, acc, create_date, last_update from [SRA_Main].[dbo].SRAFiles sf (nolock), [SRA_Main].[dbo].SRAFilesAnnotation sfa (nolock) where sf.file_id=sfa.file_id and annot_type='delite' and is_current=1 order by acc;\ngo\n" |\
#    sqsh-ms-lb -m csv -h -S SRA_BATCH -U anyone -a 1 | \
#    zstd -19 -c > "op_zq_annot2.$DATE.csv.zstd"

echo -e "SELECT acc, min(cast(create_date as Date)) as min_date from [SRA_Main].[dbo].SRAFiles sf (nolock), [SRA_Main].[dbo].SRAFilesAnnotation sfa (nolock) where sf.file_id=sfa.file_id and annot_type='delite' and is_current=1 group by acc order by min_date, acc;\ngo\n" |\
    sqsh-ms-lb -m csv -h -S SRA_BATCH -U anyone -a 1 | \
    zstd -19 -c > "op_zq_annot3.$DATE.csv.zstd"

zstd -d -c "op_zq_annot3.$DATE.csv.zstd" > "op_zq_annot3.csv"


find "$VASTFS"/sra_main/ -type f -mtime +10 -delete
