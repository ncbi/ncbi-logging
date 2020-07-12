#!/bin/bash

cd "$HOME/ncbi-logging/scripts" || exit

# shellcheck source=strides_env.sh
. ./strides_env.sh

set +e
date; echo "mirror.sh GS"
./mirror.sh GS | ts >> "$HOME"/mirror_gs.log 2>&1
date; echo "mirror.sh OP"
./mirror.sh OP | ts >> "$HOME"/mirror_op.log 2>&1
date; echo "mirror.sh S3"
./mirror.sh S3 | ts >> "$HOME"/mirror_s3.log 2>&1

date; echo "mirror_op.sh"
./mirror_op.sh | ts >> "$HOME"/mirror_op.log 2>&1

date; echo "parse S3"
./parse.sh S3  | ts >> "$HOME"/parse_s3.log 2>&1
date; echo "parse GS"
./parse.sh GS  | ts >> "$HOME"/parse_gs.log 2>&1
date; echo "s3_lister"
./s3_lister.sh | ts >> "$HOME"/s3_lister.log 2>&1
date; echo "bigquery_export"
./bigquery_export.sh | ts >> "$HOME"/bigquery_export.log 2>&1
./bigquery_report.sh | ts >> "$HOME"/bigquery_report.log 2>&1

echo "--- Daily Processing Complete ---"
date
