#!/bin/bash

. $HOME/strides/strides_env.sh

echo "Starting"
date
export DATE=$(date "+%Y%m%d") #_%H%M%S

./blast_dev.sh >> /tmp/mike_logs/blast_dev.log 2>&1
./sra_hackathon.sh >> /tmp/mike_logs/sra_hackathon.log 2>&1
./blast_hackathon.sh >> /tmp/mike_logs/blast_hackathon.log 2>&1
./sra_prod.sh >> /tmp/mike_logs/sra_prod.log 2>&1
./sra_gs.sh >> /tmp/mike_logs/sra_gs.log 2>&1
./sra_s3.sh >> /tmp/mike_logs/sra_s3.log 2>&1
./bigquery_export.sh >> /tmp/mike_logs/bigquery_export.log 2>&1

echo "Finished"
date
