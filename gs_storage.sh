#!/bin/bash

# shellcheck source=/home/vartanianmh/strides/strides_env.sh
. "$HOME/strides/strides_env.sh"

if [ "$#" -ne 2 ]; then
    echo "Usage: gs_storage.sh BUCKET SOURCE"
    exit 1
fi
BUCKET=$1
SOURCE=$2

#gsutil ls >> dus
SIZE=$(gsutil du -s "$BUCKET" | cut -d' ' -f1)
echo "$SIZE"

NOW=$(date +%s)
LINE=$(printf "%s\t%ld\t%s\t%s" "$NOW" "$SIZE" "$BUCKET" "$SOURCE")
echo "$LINE"
curl --data-binary "$LINE" "http://$GUNIHOST:$GUNIPORT/storage_tsv"

