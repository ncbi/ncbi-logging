#!/bin/bash

cd "$HOME/ncbi-logging/scripts" || exit

# shellcheck source=strides_env.sh
. ./strides_env.sh

./mirror.sh S3 | ts >> "$HOME"/mirror_s3.log 2>&1
./mirror.sh S3 | ts >> "$HOME"/mirror_s3.log 2>&1
./mirror_op.sh | ts >> "$HOME"/mirror_op.log 2>&1
./mirror.sh GS | ts >> "$HOME"/mirror_gs.log 2>&1
./parse.sh S3  | ts >> "$HOME"/parse_s3.log 2>&1
./parse.sh GS  | ts >> "$HOME"/parse_gs.log 2>&1
./s3_lister.sh | ts >> "$HOME"/s3_lister.log 2>&1
./bigquery_export.sh | ts >> "$HOME"/bigquery_export.log 2>&1
./bigquery_report.sh | ts >> "$HOME"/bigquery_report.log 2>&1

date
