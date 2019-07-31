#!/usr/bin/env python3.7

import gzip
import os
import re
import sys

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
    \s " (?P<ua>[^"]+ ) "
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
    \s " (?P<ua>[^"]+ ) "
    \s (?P<unk3>[\S]+)
    \s (?P<port>[\S]+)
    \s (?P<rl>[\S]+)
"""
prog2 = re.compile(pattern2, re.VERBOSE)

# 130.14.153.11 - - [21/Aug/2018:07:10:44 -0400] "sra-download.ncbi.nlm.nih.gov" "" 400 0 0.000 "-" "-" "-" port=443 rl=0

fout = open("csv", "w")

for fname in sys.argv[1:]:
    print("Loading %s" % fname)

    rowcount = 0
    lines = []
    if fname.endswith(".gz"):
        with gzip.open(fname) as f:
            lines = f.readlines()
    else:
        with open(fname) as f:
            lines = f.readlines()

    print("Loaded %d lines" % len(lines))

    for line in lines:
        match = prog1.match(line.decode())
        if match:
            gd = match.groupdict()
            # print ("matches: %s" % str(gd))
            row = (
                fname,
                gd["ip"],
                gd["identd"],
                gd["userid"],
                gd["finishtime"],
                gd["site"],
                gd["request"],
                gd["status"],
                gd["size"],
                gd["content"],
            )
            rowcount += 1
            print(",".join(row), file=fout)
        else:
            match = prog2.match(line.decode())
            if match:
                gd = match.groupdict()
                gd["content"] = ""
                # print ("matches: %s" % str(gd))
                row = (
                    fname,
                    gd["ip"],
                    gd["identd"],
                    gd["userid"],
                    gd["finishtime"],
                    gd["site"],
                    gd["request"],
                    gd["status"],
                    gd["size"],
                    gd["content"],
                )
                rowcount += 1
                print(",".join(row), file=fout)
            else:
                print("no match for line: %s" % line)
