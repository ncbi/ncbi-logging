#!/usr/bin/env python3

import sys
import json
from pathlib import Path
from influxdb import InfluxDBClient


def main():
    credentialfile = str(Path.home()) + "/influxdb.json"
    with open(credentialfile, "r") as fin:
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

        outrec = {
            "measurement": "detail",
            "tags": {
                "remote_ip": rec["remote_ip"],
                "host": rec["host"],
                "source": rec["source"],
            },
            "time": rec["start_ts"],
            "fields": {
                "end_ts": rec["end_ts"],
                "http_operation": rec["http_operation"],
                "request_uri": rec["request_uri"],
                "http_status": rec["http_status"],
                "bytes_sent": rec["bytes_sent"],
                "referer": rec["referer"],
                "user_agent": rec["user_agent"],
                "bucket": rec["bucket"],
            },
        }
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
