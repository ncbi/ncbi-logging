#!/usr/bin/bash

if ! curl http://intprod11:3389/ > /dev/null 2>&1
then
    mailx -s "intprod11 down" vartanianmh@ncbi.nlm.nih.gov
fi
