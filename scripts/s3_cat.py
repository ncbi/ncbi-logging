#!/usr/bin/env python3
import datetime
import dbm
import http.client
import json
from io import StringIO, BytesIO
import logging
import re
import sys
import urllib
import boto3


def main():
    if len(sys.argv) != 4:
        print(f"Usage: {sys.argv[0]} bucket prefix profile", file=sys.stderr)
        return 1

    bucket_name = sys.argv[1]
    prefix = sys.argv[2]
    profile = sys.argv[3]

    # ses = boto3.session(Session,profile_name=profile)
    boto3.setup_default_session(profile_name=profile)

    # 2019-04-03-19-42-40-C033D9617A7A978E
    s3 = boto3.client("s3")
    # bucket = s3.Bucket(bucket_name)

    #    session = boto3.session.Session(profile_name="opendata")
    #    s3 = session.client('s3')

    paginator = s3.get_paginator("list_objects_v2")
    kwargs = {"Bucket": bucket_name}
    kwargs["Prefix"] = prefix

    for page in paginator.paginate(**kwargs):
        contents = page["Contents"]
        for obj in contents:
            key = obj["Key"]
            #            print(f"key is {key}", file=sys.stderr)
            bytes_buffer = BytesIO()
            s3.download_fileobj(Bucket=bucket_name, Key=key, Fileobj=bytes_buffer)
            buf = bytes_buffer.getvalue()
            l = len(buf)
            #            print(f"size is {l}", file=sys.stderr)
            sys.stdout.buffer.write(buf)

    return 0


if __name__ == "__main__":
    main()
