#!/usr/bin/env python3

import datetime
import json
import os
import psycopg2
import sys

PCT_THRESHOLD = 3


def editdist(str1, str2):
    if len(str1) > len(str2):
        str1, str2 = str2, str1

    distances = range(len(str1) + 1)
    for i2, c2 in enumerate(str2):
        distances_ = [i2 + 1]
        for i1, c1 in enumerate(str1):
            if c1 == c2:
                distances_.append(distances[i1])
            else:
                distances_.append(
                    1 + min((distances[i1], distances[i1 + 1], distances_[-1]))
                )
        distances = distances_
    return distances[-1]


def strtodt(str1):
    try:
        start = datetime.datetime.strptime(str1, "%Y-%m-%d %H:%M:%S.%f%z")
        return start
    except ValueError:
        pass

    try:
        start = datetime.datetime.strptime(str1, "%Y-%m-%d %H:%M:%S.%z")
        return start
    except ValueError:
        pass

    try:
        start = datetime.datetime.strptime(str1, "%Y-%m-%d %H:%M:%S.%f")
        return start
    except ValueError:
        pass

    try:
        start = datetime.datetime.strptime(str1, "%Y-%m-%d %H:%M:%S")
        return start
    except ValueError:
        pass

    return None


def pctdiff(str1, str2):
    if str1 == str2:
        return 0

    str1 = str(str1)
    str2 = str(str2)
    dt1 = strtodt(str1)
    dt2 = strtodt(str2)
    if str1.isnumeric() and str2.isnumeric():
        l1 = int(str1)
        l2 = int(str2)
        avg = (l1 + l2) / 2
        avg = max(1, avg)
        diff = (max(l1, l2) - min(l1, l2)) / avg
        pct = diff * 100.0
        return pct
    # elif dt1 is not None and dt2 is not None:
    elif dt1 and dt2:
        td = dt2 - dt1
        # print(f"{str1} is {dt1}, delta is {td}")
        return float(td.days)  # Return pct as days
    else:
        pct = editdist(str1, str2)
        if pct <= 3:
            pct = 0
        pct = pct / max(len(str1), len(str2))
        return 100.0 * pct


if len(sys.argv) != 3:
    print(f"Usage: {sys.argv[0]} old.json new.json")
    sys.exit(1)

with open(sys.argv[1]) as fin:
    old = json.loads(fin.read())

with open(sys.argv[2]) as fin:
    new = json.loads(fin.read())

if len(old) != len(new):
    print("tables not in both:" + str(set(old).difference(set(new))))

tables = set(new).intersection(set(old))
for table in tables:
    for col in old[table]:
        oldcount = old[table][col]["count"]
        oldmin = old[table][col]["min"]
        oldmax = old[table][col]["max"]
        newcount = new[table][col]["count"]
        newmin = new[table][col]["min"]
        newmax = new[table][col]["max"]

        countpct = pctdiff(oldcount, newcount)
        minpct = pctdiff(oldmin, newmin)
        maxpct = pctdiff(oldmax, newmax)
        rpt = None
        if countpct > PCT_THRESHOLD:
            rpt = f"{table}.{col}.count\t{oldcount} != {newcount}\t{countpct:6.4}%"
        elif minpct > PCT_THRESHOLD:
            rpt = f"{table}.{col}.min\t{oldmin} != {newmin}\t{minpct:6.4}%"

        elif maxpct > PCT_THRESHOLD:
            rpt = f"{table}.{col}.max\t{oldmax} != {newmax}\t{maxpct:6.4}%"

        if rpt:
            flds = rpt.split("\t")
            print(f"{flds[2]:8} {flds[0]:30} {flds[1]:20}")


# print(str(old))
