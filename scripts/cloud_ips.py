#!/usr/bin/env python3

# Usage:
# 1) update uniq_ips below
# 2) https://www.microsoft.com/en-us/download/details.aspx?id=56519
SERVICE_TAGS = "https://download.microsoft.com/download/7/1/D/71D86715-5596-4529-9B13-DA13A5DE5B63/ServiceTags_Public_20210412.json"
# 3) cloud_ips.py > $PANFS/cloud_ips.jsonl
# 4) run rdns.sql


# TODO: Rewrite to check uniq_ips against CIDR ranges, rather than extensively
# listing

import json
import sys
import urllib.request

import netaddr

# dig txt _cloud-netblocks.googleusercontent.com +short
# for x in $(seq 5); do
# nslookup -q=TXT _cloud-netblocks$x.googleusercontent.com >> f
# done

#  cat f | tr ' ' '\n' | grep ip4: | cut -d':' -f 2- | sort -n

GCP_CIDRS = (
    "8.34.208.0/20 "
    "8.35.192.0/21 "
    "8.35.200.0/23 "
    "23.236.48.0/20 "
    "23.251.128.0/19 "
    "34.100.0.0/16 "
    "34.102.0.0/15 "
    "34.104.0.0/14 "
    "34.124.0.0/18 "
    "34.124.64.0/20 "
    "34.124.80.0/23 "
    "34.124.84.0/22 "
    "34.124.88.0/23 "
    "34.124.92.0/22 "
    "34.125.0.0/16 "
    "34.64.0.0/11 "
    "34.96.0.0/14 "
    "35.184.0.0/14 "
    "35.188.0.0/15 "
    "35.190.0.0/17 "
    "35.190.128.0/18 "
    "35.190.192.0/19 "
    "35.190.224.0/20 "
    "35.190.240.0/22 "
    "35.192.0.0/14 "
    "35.196.0.0/15 "
    "35.198.0.0/16 "
    "35.199.0.0/17 "
    "35.199.128.0/18 "
    "35.200.0.0/13 "
    "35.208.0.0/13 "
    "35.216.0.0/15 "
    "35.219.192.0/24 "
    "35.220.0.0/14 "
    "35.224.0.0/13 "
    "35.232.0.0/15 "
    "35.234.0.0/16 "
    "35.235.0.0/17 "
    "35.235.192.0/20 "
    "35.235.216.0/21 "
    "35.235.224.0/20 "
    "35.236.0.0/14 "
    "35.240.0.0/13 "
    "104.154.0.0/15 "
    "104.196.0.0/14 "
    "107.167.160.0/19 "
    "107.178.192.0/18 "
    "108.170.192.0/20 "
    "108.170.208.0/21 "
    "108.170.216.0/22 "
    "108.170.220.0/23 "
    "108.170.222.0/24 "
    "108.59.80.0/20 "
    "130.211.16.0/20 "
    "130.211.32.0/19 "
    "130.211.4.0/22 "
    "130.211.8.0/21 "
)

# https://ip-ranges.amazonaws.com/ip-ranges.json
with urllib.request.urlopen(
    "https://ip-ranges.amazonaws.com/ip-ranges.json"
) as response:
    AWS_JSON = response.read()

with urllib.request.urlopen(SERVICE_TAGS) as response:
    AZURE_JSON = json.load(response)

uniq_ips = set()
already = set()

# {"remote_ip":"18.212.9.218","ipint":"315886042"}
ips = open(
    "/panfs/traces01.be-md.ncbi.nlm.nih.gov/strides-analytics/uniq_ips/uniq_ips.20210422.private.json"
)
for line in ips:
    j = json.loads(line)
    uniq_ips.add(j["remote_ip"])

num_uniq_ips = len(uniq_ips)
print(f"Loaded {num_uniq_ips} uniq_ips", file=sys.stderr)

for value in AZURE_JSON["values"]:
    name = value["name"]
    name = name.replace("Azure", "")
    for prefix in value["properties"]["addressPrefixes"]:
        # print(f"Azure prefix {prefix}", file=sys.stderr)
        nums = int(prefix.split("/")[1])
        if nums >= 40 and nums <= 80:
            print(f"Azure prefix {prefix} too big:{nums}", file=sys.stderr)
            continue
        for ip in netaddr.IPNetwork(prefix):
            if str(ip) in uniq_ips and str(ip) not in already:
                print(
                    json.dumps(
                        {"ip": str(ip), "domain": f"microsoft.com (Azure {name})"}
                    )
                )
                already.add(str(ip))


# NOTE: Has duplicates (18.208.0.0/13 is "AMAZON" and "EC2"
EC = json.loads(AWS_JSON)
for prefix in EC["prefixes"]:
    ip_prefix = prefix["ip_prefix"]
    service = prefix["service"].title()
    if service == "Ec2":
        service = "EC2"
    for ip in netaddr.IPNetwork(ip_prefix):
        if str(ip) in uniq_ips and str(ip) not in already:
            print(json.dumps({"ip": str(ip), "domain": f"amazon.com (AWS {service})"}))
            already.add(str(ip))


for ipr in GCP_CIDRS.split(" "):
    if len(ipr) < 5:
        continue

    for ip in netaddr.IPNetwork(ipr):
        if str(ip) in uniq_ips and str(ip) not in already:
            print(json.dumps({"ip": str(ip), "domain": "googleusercontent.com (GCP)"}))
            already.add(str(ip))
