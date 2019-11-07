#!/bin/bash

# shellcheck source=/home/vartanianmh/strides/strides_env.sh
. "$HOME/strides/strides_env.sh"

echo "Starting"

./blast_dev.sh >> "$LOGDIR"/blast_dev.log 2>&1
./sra_hackathon.sh >> "$LOGDIR"/sra_hackathon.log 2>&1
./blast_hackathon.sh >> "$LOGDIR"/blast_hackathon.log 2>&1
./sra_gs.sh >> "$LOGDIR"/sra_gs.log 2>&1
./sra_s3.sh >> "$LOGDIR"/sra_s3.log 2>&1
./sra_prod.sh >> "$LOGDIR"/sra_prod.log 2>&1
./bigquery_export.sh >> "$LOGDIR"/bigquery_export.log 2>&1

echo "Finished"
date
