#!/usr/bin/env python3
import datetime
import os
import sys
import datetime

base = ""
if len(sys.argv) == 2:
    base = sys.argv[1]

linecount = 0
eof = False
while not eof:
    line = sys.stdin.buffer.readline()
    if len(line) == 0:
        eof = True
    # for line in sys.stdin.buffer.readlines():
    if linecount % 1000000 == 0:
        now = datetime.datetime.now()
        nowstr = now.strftime("%Y%m%d%H%M%S%f")
        fout = open(f"{base}{nowstr}.log", "wb")
    #        print(linecount, file=sys.stderr)
    #        print(nowstr, file=sys.stderr)
    #        print(line, file=sys.stderr)
    linecount += 1
    fout.write(line)
