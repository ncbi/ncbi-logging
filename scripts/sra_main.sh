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

cut -d, -f 1 sra_buckets."$DATE".csv  > bucket_today
cut -d, -f 1 sra_buckets."$YESTERDAY".csv > bucket_yesterday

if ! diff bucket_today bucket_yesterday; then
    diff bucket_today bucket_yesterday | mailx -s "Bucket Difference" vartanianmh@ncbi.nlm.nih.gov
fi


echo -e "select * from [SRA_Main].[dbo].[SRAFiles] (nolock) order by acc,file_id;\ngo\n" |
    sqsh-ms-lb -m csv -h -S SRA_BATCH -U anyone -a 1 |
    zstd -19 -c > "sramain.$DATE.csv.zstd"

#echo -e "SELECT acc, min(cast(create_date as Date)) as min_date from [SRA_Main].[dbo].SRAFiles sf (nolock), [SRA_Main].[dbo].SRAFilesAnnotation sfa (nolock) where sf.file_id=sfa.file_id and annot_type='delite' and is_current=1 group by acc order by min_date, acc;\ngo\n" |\
#    sqsh-ms-lb -m csv -h -S SRA_BATCH -U anyone -a 1 | \
#    zstd -9 -c > "op_zq_annot3.$DATE.csv.zstd"

echo -e "SELECT acc, min(cast(last_update as Date)) as min_date from [SRA_Main].[dbo].SRAFiles sf (nolock), [SRA_Main].[dbo].SRAFilesAnnotation sfa (nolock) where sf.file_id=sfa.file_id and annot_type='delite' and is_current=1 group by acc order by min_date, acc;\ngo\n" |\
    sqsh-ms-lb -m csv -h -S SRA_BATCH -U anyone -a 1 | \
    zstd -9 -c > "op_zq_annot4.$DATE.csv.zstd"

#zstd -d -c "op_zq_annot3.$DATE.csv.zstd" > "op_zq_annot3.csv"
zstd -d -c "op_zq_annot4.$DATE.csv.zstd" > "op_zq_annot4.csv"

ZQCNT=$(wc -l op_zq_annot4.csv | cut -d' ' -f 1)

if [ "$ZQCNT" -lt 44441225 ]; then
    echo "$ZQCNT" | mailx -s "Low zq4 count $ZQCNT" vartanianmh@ncbi.nlm.nih.gov
fi

find "$VASTFS"/sra_main/ -type f -mtime +10 -delete
