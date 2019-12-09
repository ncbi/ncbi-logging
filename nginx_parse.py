#!/usr/bin/env python3
import csv
import datetime
import json
import os
import re
import sys
import time

# Not sure if NCBI deviates from standrad:
#      http://httpd.apache.org/docs/1.3/logs.html
# Client IP
# identd
# userid
# finishtime
# request
# status code
# size
# referrer
# ?
#
# 220.243.135.142 - - [21/Aug/2018:00:01:03 -0400] "ftp.ncbi.nlm.nih.gov" "GET /gene/GeneRIF/ HTTP/1.1" 301 258 0 "-" "Mozilla/5.0 (Linux; Android 6.0; Nexus 5 Build/MRA58N) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/65.0.3325.181 Mobile Safari/537.36" "-" -pct 1340 - "NCBI-SID: -" port=80 258 492 text/html
pattern1 = r"""
    (?P<ip>\d{1,3} \. \d{1,3} \. \d{1,3} \. \d{1,3} )
    \s (?P<identd>[\S]+ )
    \s (?P<userid>[\S]+ )
    \s \[ (?P<finishtime>[^\]]+ ) \]
    \s " (?P<site>[^"]+ ) "
    \s " (?P<request>[^"]* ) "
    \s (?P<status>\d+)
    \s (?P<size>\d+)
    \s (?P<time>[\d\.]+)
    \s " (?P<unk2>[^"]+ ) "
    \s " (?P<agent>[^"]* ) "
    \s (?P<unk3>[\S]+)
    \s (?P<pct>[\S]+)
    \s (?P<unk4>[\S]+)
    \s (?P<unk5>[\S]+)
    \s " (?P<ncbisid>[^"]+ ) "
    \s (?P<port>[\S]+)
    \s (?P<unk6>\d+)
    \s (?P<unk7>\d+)
    \s (?P<content>[\S]+)
"""
prog1 = re.compile(pattern1, re.VERBOSE)

# 193.62.122.61 - - [21/Aug/2018:00:18:10 -0400] "sra-download.ncbi.nlm.nih.gov" "GET /traces/era4/ERR/ERR260/ERR260173 HTTP/1.1" 206 32768 0.000 "-" "linux64 sra-toolkit fastq-dump.2.9.1" "-" port=443 rl=293
# 14.204.63.15 - - [11/Dec/2018:00:19:17 -0500] "sra-download.ncbi.nlm.nih.gov" "GET /traces/sra8/SRR/000730/SRR747964 HTTP/1.1" 206 1757757162 10245.774 "-" "" "-" port=443 rl=186
pattern2 = r"""
    (?P<ip>\d{1,3} \. \d{1,3} \. \d{1,3} \. \d{1,3} )
    \s (?P<identd>[\S]+ )
    \s (?P<userid>[\S]+ )
    \s \[ (?P<finishtime>[^\]]+ ) \]
    \s " (?P<site>[^"]+ ) "
    \s " (?P<request>[^"]* ) "
    \s (?P<status>\d+)
    \s (?P<size>\d+)
    \s (?P<time>[\d\.]+)
    \s " (?P<unk2>[^"]+ ) "
    \s " (?P<agent>[^"]* ) "
    \s (?P<unk3>[\S]+)
    \s (?P<port>[\S]+)
    \s (?P<rl>[\S]+)
"""
prog2 = re.compile(pattern2, re.VERBOSE)

# 41.76.3.193 - - [28/Nov/2018:23:59:57 -0500] "ftp.ncbi.nlm.nih.gov" "HEAD /sra/refseq/NC_000069.6 HTTP/1.1" 301 - 0 "-" "linux64 sra-toolkit fastq-dump.2.9.1" "-" -pct 1624 + "NCBI-SID: -" port=80 138 201 text/html

pattern3 = r"""
    (?P<ip>\d{1,3} \. \d{1,3} \. \d{1,3} \. \d{1,3} )
    \s (?P<identd>[\S]+ )
    \s (?P<userid>[\S]+ )
    \s \[ (?P<finishtime>[^\]]+ ) \]
    \s " (?P<site>[^"]+ ) "
    \s " (?P<request>[^"]* ) "
    \s (?P<status>\d+ )
    \s (?P<unk>[\S]+)
    \s (?P<size>\d+ )
    \s " (?P<unk2>\S+) "
    \s " (?P<agent>[^"]* ) "
    \s (?P<pct>[\S]+)
    \s (?P<unk5>[\S]+)
    \s " (?P<ncbisid>[^"]+ ) "
    \s (?P<port>[\S]+)
    \s (?P<unk6>\d+)
    \s (?P<unk7>\d+)
    \s (?P<content>[\S]+)
"""
prog3 = re.compile(pattern3, re.VERBOSE)

for line in sys.stdin:
    gd = {}
    line = line.rstrip()
    match = prog1.match(line)
    if match:
        gd = match.groupdict()
    else:
        match = prog2.match(line)
        if match:
            gd = match.groupdict()
        else:
            match = prog3.match(line)
            if match:
                gd = match.groupdict()
            else:
                print("  no match for line: %s" % line, file=sys.stderr)

    ip = gd["ip"]
    acc = gd["request"]
    agent = gd["agent"]
    domain = gd["site"]
    res = gd["status"]
    start = datetime.datetime.strptime(gd["finishtime"], "%d/%b/%Y:%H:%M:%S %z")
    end = start + datetime.timedelta(seconds=float(gd["time"]))
    bytecount = gd["size"]

    s = {}
    s["ip"] = ip
    s["acc"] = acc
    s["agent"] = agent
    s["domain"] = domain
    s["res"] = res
    s["start"] = str(start)
    s["end"] = str(end)
    s["time"] = gd["time"]
    s["bytecount"] = bytecount

    print(json.dumps(s))
