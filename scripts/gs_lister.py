#!/usr/bin/env python3
import sys
import urllib
import datetime
import json

from google.cloud import storage

# export GOOGLE_APPLICATION_CREDENTIALS=/home/vartanianmh/requester-pa
# export CLOUDSDK_CORE_PROJECT="research-sra-cloud-pipeline"
# gsutil ls gs://sra-pub-src-1/
# gsutil -u research-sra-cloud-pipeline ls -l gs://sra-pub-src-1/


def main():
    if len(sys.argv) != 3:
        print(f"Usage: {sys.argv[0]} bucket profile", file=sys.stderr)
        return 1

    bucket_name = sys.argv[1]
    profile = sys.argv[2]

    bucket = storage.Client().bucket(bucket_name, user_project=profile)

    log_files = bucket.list_blobs()

    now = datetime.datetime.now()
    now = now.strftime("%Y-%m-%d %H:%M:%S")

    for log_file in log_files:
        # print (log_file)
        # print (log_file.id)
        # print (dir(log_file))
        d = {}
        key = log_file.id
        lastmodified = log_file.updated
        lastmodified = lastmodified.strftime("%Y-%m-%d %H:%M:%S")
        etag = log_file.etag
        etag = etag.replace('"', "")
        storageclass = log_file.storage_class
        size = log_file.size
        md5 = log_file.md5_hash
        d["now"] = now
        d["bucket"] = bucket_name
        d["source"] = "GS"
        d["key"] = key
        d["lastmodified"] = lastmodified
        d["etag"] = etag
        d["size"] = size
        d["storageclass"] = storageclass
        d["md5"] = md5
        print(json.dumps(d))

    return 0


if __name__ == "__main__":
    main()
