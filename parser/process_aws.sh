#!/bin/bash
set -e

CATCMD=$1
SRC_PATH=$2

for f in $( ls ${SRC_PATH} ); do
    echo "processing: $f"
    ${CATCMD} $f | ./bin/log2jsn aws > /dev/null
done