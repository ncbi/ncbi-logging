#!/usr/bin/env python3

import datetime
import sys
import json
from pathlib import Path
from influxdb import InfluxDBClient


def parsedt(str):
    # 2019-12-31 00:09:27 UTC
    # 2020-05-27T22:35:51.889
    # 2019-12-29T12:45:57
    if "UTC" in str:
        dt = datetime.datetime.strptime(str, "%Y-%m-%d %H:%M:%S %Z")
    elif "." in str:
        dt = datetime.datetime.strptime(str, "%Y-%m-%dT%H:%M:%S.%f")
    else:
        dt = datetime.datetime.strptime(str, "%Y-%m-%dT%H:%M:%S")
    return dt.isoformat()


def main():
    credentialfile = str(Path.home()) + "/influxdb.json"
    with open(credentialfile) as fin:
        credentials = json.load(fin)

    host = "influxdb-http.service.bethesda-prod.consul.ncbi.nlm.nih.gov"
    port = 8086
    user = credentials["db_user"]
    password = credentials["db_password"]
    dbname = "strides_logging"

    client = InfluxDBClient(host, port, user, password, dbname)

    json_body = []
    reccount = 0
    for line in sys.stdin:
        reccount += 1
        rec = json.loads(line)

        if "accession" in rec:
            outrec = {
                "measurement": "summary_gs_v011",
                "tags": {"source": rec["source"],},
                "time": parsedt(rec["start_ts"]),
                "fields": {
                    "bucket": rec["bucket"],
                    "host": rec["host"],
                    "remote_ip": rec["remote_ip"],
                    "num_requests": rec["num_requests"],
                    "end_ts": parsedt(rec["end_ts"]),
                    "http_operations": rec["http_operations"],
                    "accession": rec["accession"],
                    "http_statuses": rec["http_statuses"],
                    "bytes_sent": rec["bytes_sent"],
                    "referers": rec["referers"],
                    "user_agent": rec["user_agent"],
                    "city_name": rec["city_name"],
                    "country": rec["country"],
                    "region": rec["region"],
                    "domain": rec["domain"],
                },
            }
        elif "http_operation" in rec:
            outrec = {
                "measurement": "detail",
                "tags": {
                    "remote_ip": rec["remote_ip"],
                    "host": rec["host"],
                    "source": rec["source"],
                },
                "time": parsedt(rec["start_ts"]),
                "fields": {
                    "end_ts": parsedt(rec["end_ts"]),
                    "http_operation": rec["http_operation"],
                    "request_uri": rec["request_uri"],
                    "http_status": rec["http_status"],
                    "bytes_sent": rec["bytes_sent"],
                    "referer": rec["referer"],
                    "user_agent": rec["user_agent"],
                    "bucket": rec["bucket"],
                },
            }
        elif "first_appearance_provider" in rec:
            outrec = {
                "measurement": "object_appearance_v1",
                "tags": {"source": rec["source"], "bucket": rec["bucket"]},
                "time": parsedt(rec["lastmodified"]),
                "fields": {
                    "key": rec["key"],
                    "size": int(rec["size"]),
                    "checksum": rec["checksum"],
                    "first_appearance_provider": parsedt(
                        rec["first_appearance_provider"]
                    ),
                },
            }
        elif "storageclass" in rec:
            outrec = {
                "measurement": "object_delta",
                "tags": {"key": rec["key"], "source": rec["source"]},
                "time": parsedt(rec["lastmodified"]),
                "fields": {
                    "storageclass": rec["storageclass"],
                    "bucket": rec["bucket"],
                    "size": int(rec["size"]),
                    "etag": rec["etag"],
                },
            }
        else:
            print(f"Unknown record type {rec}", file=sys.stderr)
            continue
            # sys.exit(1)

        # print(outrec)
        # sys.exit(1)
        json_body.append(outrec)
        if len(json_body) >= 1000:
            if not client.write_points(json_body):
                print(f"Failed writing {json_body}", file=sys.stderr)
            json_body = []
            print(f"POSTed {reccount} records", file=sys.stderr)

    if not client.write_points(json_body):
        print(f"Failed writing {json_body}", file=sys.stderr)

    print(f"Wrote {reccount} records", file=sys.stderr)


if __name__ == "__main__":
    main()
