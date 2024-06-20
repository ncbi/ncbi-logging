#!/usr/bin/env python3.11
import sys
from io import BytesIO

import boto3


def main():
    if len(sys.argv) != 4:
        print(f"Usage: {sys.argv[0]} bucket prefix profile", file=sys.stderr)
        return 1

    bucket_name = sys.argv[1]
    prefix = sys.argv[2]
    profile = sys.argv[3]

    boto3.setup_default_session(profile_name=profile)

    s3 = boto3.client("s3")

    paginator = s3.get_paginator("list_objects_v2")
    kwargs = {"Bucket": bucket_name}
    kwargs["Prefix"] = prefix

    print(f"Extracting s3://{bucket_name}/{prefix} ...", file=sys.stderr)
    count = 0
    bytecount = 0
    truncated = False
    for page in paginator.paginate(**kwargs):
        contents = page.get("Contents", [])
        for obj in contents:
            key = obj["Key"]
            bytes_buffer = BytesIO()
            if bytecount < 50 * 1024 * 1024 * 1024:
                s3.download_fileobj(Bucket=bucket_name, Key=key, Fileobj=bytes_buffer)
                buf = bytes_buffer.getvalue()
                bytecount += len(buf)
                sys.stdout.buffer.write(buf)
                count += 1
            else:
                truncated = True
    print(
        f"Extracted {count} objects, {bytecount} bytes {truncated=}, from s3://{bucket_name}/{prefix}",
        file=sys.stderr,
    )

    return 0


if __name__ == "__main__":
    main()
