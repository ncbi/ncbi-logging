#!/usr/bin/env python3.7
import datetime
import json
import os
import time
import http.client
import csv
import dbm

# gcloud auth activate-service-account --key-file $GOOGLE_APPLICATION_CREDENTIALS

from google.cloud import storage


# Bucket is yan-blastdb-access-logs
# yan-blastdb_usage_2018_11_09_14_00_00_
# "1541774735008347","130.14.9.132","1","","GET","/yan-blastdb/2018-10-30-01-10-14/nr_v5.002.pnd","200","0","79828648","1285000","storage.googleapis.com","","Mozilla/5.0 (Windows NT 10.0; WOW64; Trident/7.0; rv:11.0) like Gecko,gzip(gfe)","AEnB2Urfle-Ryacd-69kKfNA3V0B5MkIaB4sXGh72T5XuucqiwtsTc46lXmSvi_2oYQ6QInxrasuVSuEdljOQFIxe5yXfqULJQ","GET_Object","yan-blastdb","2018-10-30-01-10-14/nr_v5.002.pnd"

# GLOBALS
LOG_BUCKET = "strides-hackathon-logs"
# STRIDES_IP="35.221.8.206"
STRIDES_IP = "localhost"
STRIDES_PORT = 20818


def get_logs(log_bucket, strides_ip, strides_port):
    storage_client = storage.Client()

    bucket = storage.Client().bucket(log_bucket)

    format = [
        "time_micros",
        "c_ip",
        "c_ip_type",
        "c_ip_region",
        "cs_method",
        "cs_uri",
        "sc_status",
        "cs_bytes",
        "sc_bytes",
        "time_taken_micros",
        "cs_host",
        "cs_referer",
        "cs_user_agent",
        "s_request_id",
        "cs_operation",
        "cs_bucket",
        "cs_object",
    ]

    log_files = bucket.list_blobs()

    conn = http.client.HTTPConnection(strides_ip, port=strides_port)

    with dbm.open("gs_agent_blast_hackathon.db", "c") as db:
        for log_file in log_files:
            if "_usage_" not in log_file.public_url:
                print(f"skipping {log_file.public_url}\n")
                continue

            if log_file.public_url in db:
                print(f"{log_file.public_url} already processed\n")
                continue

            logs = log_file.download_as_string()
            body = ""
            lines = logs.split(b"\n")
            for line in lines[1:]:
                line = line.decode()
                print("line is:" + line)
                if len(line) < 50:
                    continue
                line = line.replace("\t", "")
                csvs = csv.reader([str(line)])
                for row in csvs:
                    cols = row

                print("cols is " + str(cols))
                c = 0
                fields = {}
                for col in format:
                    fields[col] = cols[c].replace('"', "")
                    c += 1
                print(fields)

                start = float(fields["time_micros"])
                end = start + float(fields["time_taken_micros"])
                bytes = int(fields["sc_bytes"])
                if (
                    bytes == 0
                    or fields["cs_method"] != "GET"
                    or int(fields["sc_status"]) >= 400
                ):
                    continue

                start /= 1_000_000.0
                end /= 1_000_000.0

                # /download/storage/v1/b/ncbi_sra_realign/o/ERR1620370.summary?generation=1545190393681782&alt=media
                acc = fields["cs_uri"]
                acc = acc.split("?")[0]
                acc = acc.split("/")[-1]
                # Christiam suggests ignoring extension
                acc = acc.rsplit(".", 1)[0]
                if acc == "o":
                    print("Huh" + fields["cs_uri"])
                tsv = (
                    fields["c_ip"],
                    acc,
                    fields["cs_user_agent"] + " (hackathon)",
                    fields["sc_status"],
                    fields["cs_host"],
                    str(start),
                    str(end),
                    str(bytes),
                    "1",
                )

                body += "\t".join(tsv) + "\n"
            print("\bPosting: " + body)
            conn.request("POST", "/blast_tsv", body=body)
            response = conn.getresponse()
            o = response.read().decode()
            o.replace("\n", "")
            print("HTTP Response was", response.status, response.reason, o)

            db[log_file.public_url] = ""


def main():
    get_logs(LOG_BUCKET, STRIDES_IP, STRIDES_PORT)


if __name__ == "__main__":
    main()
