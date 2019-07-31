#!/usr/bin/env python3.7
import datetime
import dbm
import http.client
from io import StringIO, BytesIO
import logging
import re
import sys
import urllib

# Usage: ./s3_agent.py sra-pub-src-1-logs 'SRA@S3'

import boto3

# GLOBALS
STRIDES_IP = "intprod11"
GUNIPORT = 20817


# TODO: UTF-8 user-agent, URL, quote escape in either
# 2019-05-25-15-20-11-1293E5CE274DF238
def unquote(cols, delim):
    newcols = []
    combine = False
    if delim == '"':
        closedelim = '"'
    if delim == "[":
        closedelim = "]"
    x = 0
    while x < len(cols):
        if cols[x].startswith(delim):
            combine = True
            if cols[x][-1] == closedelim:
                cols[x] = cols[x][:-1]
                combine = False
            newcols.append(cols[x][1:])
            x += 1
            continue

        if combine:
            if cols[x][-1] == closedelim:
                combine = False
                cols[x] = cols[x][:-1]

            newcols[-1] = newcols[-1] + " " + cols[x]
        else:
            newcols.append(cols[x])

        x += 1

    return newcols


def process_log(strides_ip, strides_port, source):
    #    logger = logging.getLogger(__name__)
    s3format = [
        "bucket_owner",
        "bucket",
        "time",
        "remote_ip",
        "requester",
        "request_id",
        "operation",
        "key",
        "request_uri",
        "http_status",
        "error_code",
        "bytes_sent",
        "object_size",
        "total_time",
        "turn_around_time",
        "referrer",
        "user_agent",
        "version_id",
        "host_id",
        "signature_version",
        "cipher_suite",
        "authentication_type",
        "host_header",
        "tls_version",
    ]

    acc_re = re.compile(r"[DES]RR[0-9]{6,8}")

    sessions = {}
    linecount = 0
    for line in sys.stdin:
        if linecount % 1000 == 0:
            print("\t%d: Have %d sessions" % (linecount, len(sessions)))
        linecount += 1
        # line = line.decode()
        # logging.debug("line is:" + line)
        if len(line) < 50:
            continue
        if "\t" in line:
            line.replace("\t", "")
            logging.warning(f"Warning, tab in line")
        cols = line.split(" ")

        # logging.debug("cols is " + str(cols))
        cols = unquote(cols, "[")
        cols = unquote(cols, '"')
        # logging.info("cols is now " + str(cols))

        c = 0
        fields = {}
        for col in s3format:
            fields[col] = cols[c]
            c += 1
        # logging.info("fields are:" + str(fields))

        acc = fields["request_uri"]  # .split(" ")[1]
        if " " not in acc:
            continue
        acc = acc.split(" ")
        cmd = acc[0].upper()
        acc = acc[1]
        if cmd != "GET" and cmd != "HEAD":
            continue
        # logging.info(f"ACC was {acc}")

        if fields["total_time"] == "-":
            fields["total_time"] = "1"

        start = datetime.datetime.strptime(fields["time"], "%d/%b/%Y:%H:%M:%S %z")
        end = start + datetime.timedelta(milliseconds=float(fields["total_time"]))
        if fields["bytes_sent"] == "-":
            fields["bytes_sent"] = "0"

        sc_bytes = int(fields["bytes_sent"])
        # if (
        #    sc_bytes == 0
        #    #or not fields["request_uri"].startwsith("GET")
        #    or int(fields["http_status"]) >= 400
        # ):
        #    continue

        logging.debug(f"start is {start}")
        start = start.timestamp()
        end = end.timestamp()
        logging.debug(f"start is now {start}")

        acc = urllib.parse.unquote(acc)

        if acc_re.search(acc):
            acc = acc_re.findall(acc)[0]
        else:
            continue
        # acc = acc.split("?")[0]
        # acc = acc.split("/")[-1]

        if "blast" in source.lower():
            if acc in BLAST_DBS:
                acc = acc.split(".")[0] + " db"
            else:
                acc = acc.split(".")[0] + " files"
        else:
            acc = acc  # .split(".")[0]
        # Christiam suggests ignoring extension.

        # logging.info(f"ACC  is {acc}")

        ip = fields["remote_ip"]
        agent = fields["user_agent"]
        status = fields["http_status"]
        header = fields["host_header"]
        cnt = 1
        sesskey = "%s,%s,%s" % (ip, acc, agent)

        if sesskey in sessions:
            sess = sessions[sesskey]
            # print("sess was:" + str(sess))
            sess["end"] = max(end, sess["end"])
            sess["bytecount"] = sess["bytecount"] + sc_bytes
            sess["cnt"] = sess["cnt"] + 1
            sessions[sesskey] = sess
            print("sess  is:" + str(sess))
        else:
            sess = {
                "ip": ip,
                "acc": acc,
                "agent": agent,
                "start": start,
                "end": end,
                "bytecount": sc_bytes,
                "cnt": cnt,
                "status": status,
                "host": header,
            }
            # print("sess is " + repr(sess))

            sessions[sesskey] = sess

    body = ""
    print("\tHave %d sessions" % len(sessions))

    for sesskey, sess in sessions.items():
        ip = sess["ip"]
        acc = sess["acc"]
        agent = sess["agent"]
        start = sess["start"]
        end = sess["end"]
        bytecount = sess["bytecount"]
        cnt = sess["cnt"]
        status = sess["status"]
        source = source
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

    if len(body) > 1:
        logging.info("\bPosting:\n" + body)
        if False:
            conn = http.client.HTTPConnection(strides_ip, port=strides_port)
            conn.request("POST", "/sess_start_tsv", body=body)
            response = conn.getresponse()
            o = response.read().decode()
            o.replace("\n", "")
            logging.info(f"HTTP Response was {response.status}, {response.reason}")
    #        logging.info("HTTP Response was", str(response.status), str(response.reason) )


def get_logs(source, strides_ip, strides_port):
    process_log(strides_ip, strides_port, source)


def main():
    if len(sys.argv) != 2:
        print(f"Usage: {sys.argv[0]} source", file=sys.stderr)
        return 1

    logging.basicConfig(
        level=logging.INFO,
        format="%(asctime)s %(name)-12s %(process)d %(funcName)s %(levelname)-8s %(message)s",
        datefmt="%m-%d %H:%M",
        filename="/tmp/mike_logs/s3_agent.log",
        filemode="w",
    )
    console = logging.StreamHandler()
    console.setLevel(logging.INFO)
    formatter = logging.Formatter("%(name)-12s: %(levelname)-8s %(message)s")
    console.setFormatter(formatter)
    logging.getLogger("").addHandler(console)

    logging.basicConfig(level=logging.INFO, format="%(asctime)s %(message)s")
    get_logs(STRIDES_IP, GUNIPORT, sys.argv[1])
    return 0


if __name__ == "__main__":
    main()
