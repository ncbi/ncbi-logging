#!/bin/bash

# shellcheck source=strides_env.sh
. ./strides_env.sh

export GOOGLE_APPLICATION_CREDENTIALS=$HOME/sandbox-blast-847af7ab431a.json
gcloud config set account 1008590670571-compute@developer.gserviceaccount.com
export CLOUDSDK_CORE_PROJECT="ncbi-sandbox-blast"


export PATH="$PATH":/opt/sybase/utils/bin
mkdir -p "$PANFS/sra_main"

cd "$PANFS/sra_main" || exit

echo -e "select * from [SRA_Main].[dbo].[SRAFiles] order by acc,file_id;\ngo\n" | \
    sqsh-ms-lb -m csv -h -S SRA_BATCH -U anyone -a 1 \
    | gzip -9 -c > "sramain.$DATE.csv.gz"
