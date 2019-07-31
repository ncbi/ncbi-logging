#!/usr/bin/env python3.7
# gcloud auth activate-service-account --key-file $GOOGLE_APPLICATION_CREDENTIALS
import csv
import dbm
import http.client
import sys
import urllib

from google.cloud import storage

# Bucket is yan-blastdb-access-logs
# yan-blastdb_usage_2018_11_09_14_00_00_
# "1541774735008347","130.14.9.132","1","","GET","/yan-blastdb/2018-10-30-01-10-14/nr_v5.002.pnd","200","0","79828648","1285000","storage.googleapis.com","","Mozilla/5.0 (Windows NT 10.0; WOW64; Trident/7.0; rv:11.0) like Gecko,gzip(gfe)","AEnB2Urfle-Ryacd-69kKfNA3V0B5MkIaB4sXGh72T5XuucqiwtsTc46lXmSvi_2oYQ6QInxrasuVSuEdljOQFIxe5yXfqULJQ","GET_Object","yan-blastdb","2018-10-30-01-10-14/nr_v5.002.pnd"

# GLOBALS
# STRIDES_IP="35.221.8.206"
STRIDES_IP = "intprod11"
GUNIPORT = 20817

BLAST_DBS = {
    "nt_v5.pal",
    "nr_v5.pal",
    "NCBI_VIV_nucleotide_sequences_v5.gil",
    "NCBI_VIV_protein_sequences_v5.gil",
    "GCF_000001635.24_top_level.00.nsd",
    "GCF_000001405.38_top_level.00.nsd",
    "pdb_v5.pos",
    "refseq_protein_v5.90.msk",
    "refseq_rna_v5.20.msk",
    "prokaryotic_16S_ribosomal_RNA_v5.ngil",
    "swissprot_v5.00.pog",
    "nt_v5.nos",
    "nr_v5.ptf",
    "split-cdd.00.aux",
    "landmark_v5.pin",
    "ref_viroids_rep_genomes_v5.nin",
    "ref_viruses_rep_genomes_v5.nin",
}


def get_logs(log_bucket, source, strides_ip, strides_port):
    # storage_client = storage.Client()

    bucket = storage.Client().bucket(log_bucket)

    gsformat = [
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

    with dbm.open(log_bucket + ".db", "c") as db:
        for log_file in log_files:
            if "_usage_" not in log_file.public_url:
                print(f"skipping {log_file.public_url}")
                continue

            if log_file.public_url in db:
                print(f"{log_file.public_url} already processed\n")
                continue

            logs = log_file.download_as_string()
            body = ""
            lines = logs.split(b"\n")
            for line in lines[1:]:
                line = line.decode()
                if len(line) < 50:
                    continue
                # print("line is:" + line)
                line = line.replace("\t", "")
                csvs = csv.reader([str(line)])
                for row in csvs:
                    cols = row

                # print("cols is " + str(cols))
                c = 0
                fields = {}
                for col in gsformat:
                    fields[col] = cols[c].replace('"', "")
                    c += 1

                start = float(fields["time_micros"])
                end = start + float(fields["time_taken_micros"])
                sc_bytes = int(fields["sc_bytes"])
                if (
                    sc_bytes == 0
                    or fields["cs_method"] != "GET"
                    or int(fields["sc_status"]) >= 400
                ):
                    continue

                # print("fields are" + str(fields))

                start /= 1_000_000.0
                end /= 1_000_000.0

                # /download/storage/v1/b/ncbi_sra_realign/o/ERR1620370.summary?generation=1545190393681782&alt=media
                # /download/storage/v1/b/blast-db/o/2019-01-02-11-16-10%2Fref_viruses_rep_genomes_v5.nin?generation=15464
                acc = fields["cs_object"]
                acc = urllib.parse.unquote(acc)
                acc = acc.split("?")[0]
                acc = acc.split("/")[-1]

                if "blast" in source.lower():
                    if acc in BLAST_DBS:
                        acc = acc.split(".")[0] + " db"
                    else:
                        acc = acc.split(".")[0] + " files"
                else:
                    acc = acc.split(".")[0]
                # Christiam suggests ignoring extension.

                if len(acc) < 4:
                    continue
                print("ACC  is", acc)

                tsv = (
                    fields["c_ip"],
                    acc,
                    fields["cs_user_agent"],
                    fields["sc_status"],
                    fields["cs_host"],
                    str(start),
                    str(end),
                    str(sc_bytes),
                    "1",
                    source,
                )

                body += "\t".join(tsv) + "\n"

            if len(body) > 10:
                print("\bPosting: " + body)
                conn.request("POST", "/sess_start_tsv", body=body)
                response = conn.getresponse()
                o = response.read().decode()
                o.replace("\n", "")
                print("HTTP Response was", response.status, response.reason, o)

            print(f"{log_file.public_url} processed\n")
            db[log_file.public_url] = ""


def main():
    if len(sys.argv) != 3:
        print(f"Usage: {sys.argv[0]} bucket source", file=sys.stderr)
        return 1
    get_logs(sys.argv[1], sys.argv[2], STRIDES_IP, GUNIPORT)
    return 0


if __name__ == "__main__":
    main()
