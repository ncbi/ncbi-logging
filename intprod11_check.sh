#!/usr/bin/bash

curl http://intprod11:3389/ > /dev/null 2>&1
if [ $? -ne 0 ]; then
    mailx -s "intprod11 down" vartanianmh@ncbi.nlm.nih.gov
fi

