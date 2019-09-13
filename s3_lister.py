#!/usr/bin/env python3.7
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
    if len(sys.argv) != 2:
        print(f"Usage: {sys.argv[0]} bucket", file=sys.stderr)
        return 1

    bucket_name = sys.argv[1]

    s3 = boto3.client("s3")
    #    session = boto3.session.Session(profile_name="opendata")
    #    s3 = session.client('s3')

    paginator = s3.get_paginator("list_objects_v2")

    now = datetime.datetime.now()
    now = now.strftime("%Y-%m-%d %H:%M:%S")

    for page in paginator.paginate(Bucket=bucket_name):
        contents = page["Contents"]
        for obj in contents:
            key = obj["Key"]
            LastModified = obj["LastModified"]
            LastModified = LastModified.strftime("%Y-%m-%d %H:%M:%S")
            ETag = obj["ETag"]
            ETag = ETag.replace('"', "")
            Size = obj["Size"]
            StorageClass = obj["StorageClass"]
            d = {}
            d["now"] = now
            d["bucket"] = bucket_name
            d["source"] = "S3"
            d["key"] = key
            d["lastmodified"] = LastModified
            d["etag"] = ETag
            d["size"] = Size
            d["storageclass"] = StorageClass
            d["md5"] = Etag
            #            print (obj)
            print(json.dumps(d))
            # 'Key': 'DRR000871/DRR000871.1', 'LastModified': datetime.datetime(2019, 4, 7, 14, 42, 5, tzinfo=tzutc()), 'ETag': '"c148c3c12ff4ff3353f4d1908cf9a0d6-697"', 'Size': 5846499276, 'StorageClass': 'STANDARD'

    return 0


if __name__ == "__main__":
    main()
