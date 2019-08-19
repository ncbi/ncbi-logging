#!/usr/bin/env python3.7
import csv
import datetime
import dbm
import http.client
import json
import os
import re
import sys
import time

# GLOBALS
# STRIDES_IP="35.221.8.206"
STRIDES_IP = "intprod11"
GUNIPORT = 20817


def get_logs(strides_ip, strides_port):
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
    conn = http.client.HTTPConnection(strides_ip, port=strides_port)

    # 14.204.63.15 - - [06/Dec/2018:08:15:06 -0500] "sra-download.ncbi.nlm.nih.gov" "GET /traces/sra8/SRR/000730/SRR748177 HTTP/1.1" 206 48152299 1321.691 "-" "" "-" port=443 rl=186
    # 128.231.2.239 - - [10/Dec/2018:23:59:51 -0500] "sra-download.ncbi.nlm.nih.gov" "GET https://sra-download.ncbi.nlm.nih.gov:443/traces/sra66/SRR/007160/SRR7332832 HTTP/1.1" 206 458752 0.000 "-" "linux64 sra-toolkit fasterq-dump.2.9.2" "-" port=443 rl=336
    # 34.222.99.140 - - [11/Dec/2018:07:36:43 -0500] "sra-download.ncbi.nlm.nih.gov" "" 400 0 0.000 "-" "-" "-" port=443 rl=0

    # 130.14.28.32 - - [03/Jun/2019:00:03:35 -0400] "sra-download.ncbi.nlm.nih.gov" "GET /srapub_files/3896971 HTTP/1.1" 301 162 0.000 "-" "Python-urllib/2.7" "-" port=80 rl=158
    # ['130.14.28.32', '-', '-', '[03/Jun/2019:00:03:35', '-0400]', '"sra-download.ncbi.nlm.nih.gov"', '"GET', '/srapub/ERR022446.vdbcache', 'HTTP/1.1"', '301', '162', '0.000', '"-"', '"Python-urllib/2.7"', '"-"', 'port=80', 'rl=163']

    acc_re = re.compile(r"[DES]RR[0-9]{6,8}")

    sessions = {}
    for line in sys.stdin:
        try:
            line = line.replace('"', "")
            line = line.rstrip()
            fields = line.split(" ")
            ip = fields[0]
            start = " ".join(fields[3:5])
            start = datetime.datetime.strptime(start, "[%d/%b/%Y:%H:%M:%S %z]")
            domain = fields[5]
            host = fields[5]
            acc = fields[7].split("/")[-1]
            acc = acc.split(".")[0]
            status = fields[9]
            res = fields[9]
            bytecount = float(fields[10])
            seconds = float(fields[11])
            end = start + datetime.timedelta(seconds=seconds)
            agent = fields[13]

            cnt = 1

            start = str(start.timestamp())
            end = str(end.timestamp())
        except Exception as e:
            print("bad line: " + line + str(e))
            print("bad line: " + repr(fields))
            continue

        if acc_re.match(acc):
            acc = acc_re.findall(acc)[0]
        else:
            print("  fields are" + repr(fields))
            print("  no acc: " + acc)
            continue

        sesskey = "%s,%s,%s,%s" % (ip, acc, agent, domain)
        if sesskey in sessions:
            sess = sessions[sesskey]
            # print("sess was:" + str(sess))
            sess["end"] = end
            sess["bytecount"] = sess["bytecount"] + bytecount
            sess["cnt"] = sess["cnt"] + 1
            sessions[sesskey] = sess
            # print("sess  is:" + str(sess))
        else:
            sess = {
                "ip": ip,
                "acc": acc,
                "agent": agent,
                "domain": domain,
                "start": start,
                "end": end,
                "bytecount": bytecount,
                "cnt": cnt,
                "status": status,
                "host": host,
            }
            print("sess is " + repr(sess))

            sessions[sesskey] = sess

    body = ""
    print("\tHave %d sessions" % len(sessions))
    for sesskey, sess in sessions.items():
        ip = sess["ip"]
        acc = sess["acc"]
        agent = sess["agent"]
        domain = sess["domain"]
        start = sess["start"]
        end = sess["end"]
        bytecount = sess["bytecount"]
        cnt = sess["cnt"]
        status = sess["status"]
        source = "SRA prod2"
        host = sess["host"]

        tsv = (
            ip,
            acc,
            agent,
            status,
            host,
            str(start),
            str(end),
            "%d" % bytecount,
            "%d" % cnt,
            source,
        )

        body += "\t".join(tsv) + "\n"

    print("\tPosting: " + body)
    if True:
        conn.request("POST", "/sess_start_tsv", body=body)
        response = conn.getresponse()
        o = response.read().decode()
        o.replace("\n", "")
        print("HTTP Response was", response.status, response.reason, o)
        body = ""

    if False:
        if len(body) > 1:
            print("\tFinal Posting: " + body)
            conn.request("POST", "/sess_start_tsv", body=body)
            r
            sponse = conn.getresponse()
            o = response.read().decode()
            o.replace("\n", "")
            print("HTTP Response was", response.status, response.reason, o)


def main():
    get_logs(STRIDES_IP, GUNIPORT)


if __name__ == "__main__":
    main()
