#!/usr/bin/env python3

import sys
from datetime import date
import datetime

duration = int(sys.argv[1])

accsize = {}
with open("accsize.csv") as f:
    for line in f:
        line = line.strip()
        (acc, size_mb) = line.split(",")
        accsize[acc] = int(size_mb) * 1024 * 1024

print("Loaded %d sizes" % len(accsize), file=sys.stderr)

expire = {}
thawbytes = 0
storebytedays = 0
hits = 0
recs = 0
warm = 0
capmiss = 0
compmiss = 0
missacc = 0
prevday = ""
ipmisses = set()
for line in sys.stdin:
    recs += 1
    warm += 1
    if warm == 1000000:
        hits = 0
        recs = 0
        thawbytes = 0
        storebytedays = 0
        capmiss = 0
        compmiss = 0
        ipmisses = set()

    line = line.strip()
    try:
        (acc, ip, start_ts, bytecount) = line.split(",")
        bytecount = int(bytecount)
        # print (acc, start_ts, bytecount)
        # start=date.fromisoformat(start_ts)
    except Exception as e:
        print("Bad line", line)
        continue
    # print (start)
    start = datetime.datetime.strptime(start_ts, "%Y-%m-%d %H:%M:%S")

    expired = False
    if acc not in expire:
        expired = True
        compmiss += 1
        # print(acc, "never")
    else:
        diff = start - expire[acc]
        # print(acc, "hit")
        # print (diff)
        if diff.total_seconds() > duration * 86400:
            expired = True
            capmiss += 1
            # print(acc, "expired")
        else:
            hits += 1

    if expired:
        expire[acc] = start
        #        if acc not in accsize:
        #            print("bad acc", acc, file=sys.stderr)
        if acc not in accsize:
            missacc += 1
            bytecount = 840 * 1024 * 1024  # Average accession is 840MB

        bytecount = accsize.get(acc, bytecount)
        thawbytes += bytecount
        storebytedays += bytecount * duration
        ipmisses.add(ip)

#    if start != prevday:
#        prevday = start
#        print(start, hits, hits / recs)
#        hits = 0
#        recs = 0

out = (
    str(duration),
    str(hits),
    str(recs),
    str(hits / recs),
    str(thawbytes),
    str(storebytedays),
    str(capmiss),
    str(compmiss),
    str(missacc),
    str(len(ipmisses)),
    str(len(expire)),
)
print(",".join(out))
