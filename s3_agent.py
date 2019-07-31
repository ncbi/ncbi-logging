#!/usr/bin/env python3.7
import datetime
import dbm
import http.client
from io import StringIO, BytesIO
from multiprocessing import Pool, Process
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


def process_log(lines, strides_ip, strides_port, log_file, source):
    #    logger = logging.getLogger(__name__)
    print(f"Processing {log_file}")
    logging.info(f"Processing {log_file}")
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

    logging.info("got " + str(len(lines)) + " lines")
    body = ""
    for line in lines:
        line = line.decode()
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
        logging.info(f"ACC was {acc}")

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

        logging.info(f"ACC  is {acc}")
        tsv = (
            fields["remote_ip"],
            acc,
            fields["user_agent"],
            fields["http_status"],
            fields["host_header"],
            str(start),
            str(end),
            str(sc_bytes),
            "1",
            source,
        )

        body += "\t".join(tsv) + "\n"

    if len(body) > 1:
        logging.info("\bPosting: " + body)
        conn = http.client.HTTPConnection(strides_ip, port=strides_port)
        conn.request("POST", "/sess_start_tsv", body=body)
        response = conn.getresponse()
        o = response.read().decode()
        o.replace("\n", "")
        logging.info(f"HTTP Response was {response.status}, {response.reason}")
    #        logging.info("HTTP Response was", str(response.status), str(response.reason) )
    logging.info(f"Processed {log_file}")


def get_logs(log_bucket, source, strides_ip, strides_port):
    logger = logging.getLogger(__name__)
    session = boto3.session.Session(profile_name="strides-analytics")
    s3 = session.resource("s3")

    bucket = s3.Bucket(log_bucket)
    log_files = bucket.objects.all()

    results = []
    with Pool(processes=8, maxtasksperchild=100) as pool:  # Default is cpu_count
        with dbm.open(log_bucket + ".db", "c") as db:
            for log_file in log_files:
                log_file = log_file.key
                if log_file in db:
                    logging.info(f"{log_file} already processed\n")
                    continue

                buf = BytesIO()
                bucket.download_fileobj(log_file, buf)
                lines = buf.getvalue().split(b"\n")

                res = pool.apply_async(
                    process_log,
                    args=(lines, strides_ip, strides_port, log_file, source),
                )
                print("submitted " + log_file)
                res.get()
                db[log_file] = ""
            pool.close()
            pool.join()


def main():
    if len(sys.argv) != 3:
        print(f"Usage: {sys.argv[0]} bucket source", file=sys.stderr)
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
    get_logs(sys.argv[1], sys.argv[2], STRIDES_IP, GUNIPORT)
    return 0


if __name__ == "__main__":
    main()
