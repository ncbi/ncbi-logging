#!/usr/bin/env python3

import sys

for line in sys.stdin.readlines():
    line = line.rstrip()
    fields = line.split(":")
    if len(fields) >= 2:
        #    print(fields)
        ip = fields[0]
        domain = fields[1]
        if len(domain) > 2:
            fqdns = domain.split(".")
            # print(fqdns)
            t3 = fqdns[:-1][-3:]
            print("%s\t%s" % (ip, ".".join(t3)))
