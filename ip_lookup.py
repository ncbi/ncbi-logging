#!/usr/bin/env python3.7

import sys
import socket
import whois

CLOUD_IPS = set()
with open("cloud_ips.tsv") as cips:
    for line in cips:
        (ip, desc) = line.split("\t")
        CLOUD_IPS.add(ip)
print("Loaded %d cloud IPs" % len(CLOUD_IPS), file=sys.stderr)

for line in sys.stdin.readlines():
    wfound = False
    domain_name = ""
    host = "unknown"
    ip = line.rstrip()
    ip = ip.replace(" ", "")
    if len(ip) < 7:
        continue
    w = None

    if ip in CLOUD_IPS:
        continue

    try:
        host = socket.gethostbyaddr(ip)[0]
        if len(host) > 2:
            fqdns = host.split(".")
            host = fqdns[-3:]
            host = ".".join(host)

            w = whois.whois(ip)
            wfound = True
    except:
        wfound = False

    if wfound:
        # print(w.keys(), file=sys.stderr)
        # print(w.domain_name, file=sys.stderr)
        # print(w.name, file=sys.stderr)
        # print(w.org, file=sys.stderr)
        # print(w.emails, file=sys.stderr)

        if w.domain_name:
            domain_name = w.domain_name
        elif w.name:
            domain_name = w.name

        if type(domain_name) is list:
            domain_name = domain_name[0]
        else:
            domain_name = domain_name

        if len(domain_name) < 4:
            if w.emails:
                domain_name = w.emails
                if type(domain_name) is list:
                    domain_name = domain_name[-1]
                domain_name = domain_name.split("@")[-1]

        if len(domain_name) < 4:
            domain_name = host

    domain_name = domain_name.lower()
    if host == "unknown" and wfound:
        host = domain_name
    elif host == "unknown" and not wfound:
        host = "unknown"
    else:
        host += " (" + domain_name + ")"

    host = host.replace(" ()", "")
    # host = host.lower()
    print("%s\t%s" % (ip, host))
    sys.stdout.flush()
