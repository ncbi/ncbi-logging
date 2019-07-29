#!/usr/bin/env python3.6
import csv
import datetime
import http.client
import json
import os
import sys
import time


ACCS = set()
for row in csv.reader(sys.stdin):
    if len(row) == 47:
        acc = row[0]
        if acc not in ACCS:
            csv.writer(sys.stdout).writerow(row)
            ACCS.add(acc)
        else:
            print(f"Duplicate {acc}", file=sys.stderr)
    else:
        print("bad row (" + str(len(row)) + "):" + str(row), file=sys.stderr)
