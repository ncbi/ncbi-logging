#!/usr/bin/env python3.13
import json
import statistics
import sys
from datetime import date, timedelta

from google.cloud import storage
from google.oauth2 import service_account

# export GOOGLE_APPLICATION_CREDENTIALS=/home/vartanianmh/requester-pa
# export CLOUDSDK_CORE_PROJECT="research-sra-cloud-pipeline"
# gsutil ls gs://sra-pub-src-1/
# gsutil -u research-sra-cloud-pipeline ls -l gs://sra-pub-src-1/


def numsum(storage_client, bucket_name, pattern):
    bucket = storage_client.bucket(bucket_name)  # , user_project=profile)

    objects = bucket.list_blobs(
        # fields="id,name,size",
        delimiter="/",
        match_glob=pattern,
    )

    num = 0
    sum = 0
    for object in objects:
        key = object.id
        name = object.name
        size = object.size
        num += 1
        sum += size
        print(f"\t{key=} {name=} {size=:,}", file=sys.stderr)

    if num == 0:
        print(f"{bucket_name}/{pattern} {num=} {sum=:,}")
    return num, sum


# TODO: unrecognized
# Public and CA/dbGaP
# Mirror
# GS
# OP
# S3
# Parse
# GS
# OP
# S3

LAG = 2
NUMDAYS = 8


def main():
    hist = {}
    for days_ago in range(LAG, LAG + NUMDAYS):  # 2,3,4,5,6
        previous_day = (date.today() - timedelta(days_ago)).strftime("%Y-%m-%d")
        print(previous_day, file=sys.stderr)

        # Project/key, bucket, pattern
        checks = []

        # MIRRORS
        # logmon_logs/gs_public/2019-03-17.sra-pub-logs-1.tar.gz/1593460448532298
        checks.append(
            {
                "project": "logmon",
                "bucket": "logmon_logs",
                "pattern": f"gs_public/{previous_day}.sra-pub*",
            }
        )

        # logmon_logs/op_public/2020-09-02.OP-srafiles21.tar.gz/1599143430977264 1300934519
        checks.append(
            {
                "project": "logmon",
                "bucket": "logmon_logs",
                "pattern": f"op_public/{previous_day}.OP-*",
            }
        )

        # s3_public/2020-06-20.sra-pub-src-1-logs.tar.gz
        checks.append(
            {
                "project": "logmon",
                "bucket": "logmon_logs",
                "pattern": f"s3_public/{previous_day}.sra-*",
            }
        )

        # logmon_logs/gs_public/2026-11-15.sra-ca-logs-1.tar.gz/1794851951206299

        # PARSED
        # gs://logmon_logs_parsed_us/logs_gs_public/v3/recognized.sra-pub-logs-1.2026-11-14.000.jsonl.gz
        checks.append(
            {
                "project": "logmon",
                "bucket": "logmon_logs_parsed_us",
                "pattern": f"logs_gs_public/v3/recognized.sra-*.{previous_day}.*.jsonl.gz",
            }
        )

        # gs://logmon_logs_parsed_us/logs_op_public/v3/recognized.OP-web.2026-11-14.039.jsonl.gz
        checks.append(
            {
                "project": "logmon",
                "bucket": "logmon_logs_parsed_us",
                "pattern": f"logs_op_public/v3/recognized.OP-*.{previous_day}.*.jsonl.gz",
            }
        )

        # gs://logmon_logs_parsed_us/logs_s3_public/v3/recognized.sra-pub-src-1-logs.2026-11-04.009.jsonl.gz
        checks.append(
            {
                "project": "logmon",
                "bucket": "logmon_logs_parsed_us",
                "pattern": f"logs_s3_public/v3/recognized.sra-*.{previous_day}.*.jsonl.gz",
            }
        )

        # dbGaP
        checks.append(
            {
                "project": "logmon",
                "bucket": "logmon_logs_parsed_private",
                "pattern": f"logs_op_private/v3/recognized.OP-*.{previous_day}.*.jsonl.gz",
            }
        )

        checks.append(
            {
                "project": "logmon",
                "bucket": "logmon_logs_parsed_private",
                "pattern": f"logs_s3_private/v3/recognized.sra-ca-*.{previous_day}.*.jsonl.gz",
            }
        )

        #        morechecks = []
        #        for check in checks:
        #            morechecks.append(check)
        #            if "recognized" in check["pattern"]:
        #                check["pattern"].replace("recognized", "unrecognized")
        #                morechecks.append(check)

        for check in checks:
            project_id = check["project"]

            #    project_id = "logmon"
            # profiles = ["ncb-logmon"]
            # project_id="ncbi-logmon" in JSON?
            with open(f"/home/vartanianmh/{project_id}.json") as source:
                info = json.load(source)

            storage_credentials = service_account.Credentials.from_service_account_info(
                info
            )
            storage_client = storage.Client(
                project=project_id, credentials=storage_credentials
            )

            bucket = check["bucket"]
            pattern = check["pattern"]

            _, sum = numsum(storage_client, bucket, pattern)
            k = f"{bucket}/{pattern}"
            k = k.split("/")
            k = "/".join(k[0:2])

            if k not in hist:
                hist[k] = [0] * (LAG + NUMDAYS)
            hist[k][days_ago] = sum

    print(hist, file=sys.stderr)

    for k, v in hist.items():
        mean = int(statistics.mean(v))
        recent = v[LAG]
        pct = recent / mean
        print(f"{k=} {mean=} {recent=} {pct}", file=sys.stderr)

        recents = "[ "
        for r in v:
            recents += f"{r:,} "
        recents += "]"

        if pct < 0.19 or pct > 4:
            print(f"Volume delta ({pct:0.2}) for {k}: {recent=:,} {mean=:,} {recents}")

    return 0


if __name__ == "__main__":
    main()
