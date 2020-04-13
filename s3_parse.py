#!/usr/bin/env python3
import datetime
import dbm
import http.client
from io import StringIO, BytesIO
from multiprocessing import Pool, Process
import logging
import re
import sys
import urllib

foo = '922194806485875312b252374a3644f1feecd16802a50d4729885c1d11e1fd37 sra-pub-run-1 [22/Mar/2019:18:05:54 +0000] 3.89.6.210 arn:aws:iam::783971887864:user/ignatovi 60EC3D4D2E9BD7DB REST.GET.BUCKET - "GET /?list-type=2&prefix=%2A&encoding-type=url HTTP/1.1" 200 - 277 - 31 30 "-" "aws-cli/1.16.129 Python/3.6.7 Linux/4.15.0-1032-aws botocore/1.12.119" - phxPRSerlPfWhPqrFteExBf5yEEJyjepC6rATLWr+Dc4kDUA1PHyWq8MCIP6za5m5Ve3tMVGhPQ= SigV4 ECDHE-RSA-AES128-GCM-SHA256 AuthHeader sra-pub-run-1.s3.amazonaws.com TLSv1.2'

line_re = re.compile(
    r'([^ ]*) ([^ ]*) [(.*?)] ([^ ]*) ([^ ]*) ([^ ]*) ([^ ]*) ([^ ]*) "([^ ]*) ([^ ]*) (- |[^ ]*)" (-|[0-9]*) ([^ ]*) ([^ ]*) ([^ ]*) ([^ ]*) ([^ ]*) ([^ ]*) ("[^"]*") ([^ ]*)(?: ([^ ]*) ([^ ]*) ([^ ]*) ([^ ]*) ([^ ]*) ([^ ]*))?.*$'
)

for line in sys.stdin:
    gd = {}
    line = line.rstrip()
    match = line_re.match(line)
    print(match)
