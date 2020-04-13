#!/usr/bin/env python3

# gunicorn3 -t 120 -b 127.0.0.1:8000 --reload myapp:app

# curl -v --data-binary "@data.txt" http://127.0.0.1:80/sess_start_tsv
# 1.119.142.196     SRR1972739  linux64 sra-toolkit fastq-dump.2.8.2      sra-download.ncbi.nlm.nih.gov     200   1539891273.321    1538981273.432    0     2\n

import sys
import ipaddress
import traceback
from io import BytesIO
import datetime
import psycopg2


def app(environ, start_response):
    output = BytesIO()
    try:
        if environ["PATH_INFO"] == "/sess_start_tsv":
            sess_start_tsv(environ, "add_session", start_response, output)
            return [output.getvalue()]
        elif environ["PATH_INFO"] == "/storage_tsv":
            storage_tsv(environ, start_response, output)
            return [output.getvalue()]
    except:
        headers = [("content-type", "text/plain")]
        start_response("500 oops", headers, sys.exc_info())
        print(sys.exc_info(), file=sys.stderr)
        print(traceback.print_tb(sys.exc_info()[2]))
        return [b"exception"]


def storage_tsv(environ, start_response, output):
    headers = []
    headers.append(("Content-Type", "text/plain"))

    if environ["REQUEST_METHOD"].upper() != "POST":
        output.write(b"Only POST supported\n")
        dump_headers(environ, start_response, output)
        return

    start_response("200 OK", headers)

    output.write(b"\nOutput\n------\n")
    request_len = int(environ.get("CONTENT_LENGTH", 0))
    output.write(b"Got %d input bytes\n" % request_len)

    wsgi_input = environ["wsgi.input"]
    count = 0
    try:
        db = psycopg2.connect(
            "dbname='grafana' user='grafana' host='localhost' password='grafana'"
        )
        curs = db.cursor()
    except Exception as e:
        print("can't connect to the database: " + str(e))

    r = wsgi_input.read()
    lines = r.split(b"\n")
    output.write(b"Got %d lines\n" % len(lines))
    for line in lines:
        if line is None:
            print("Done", file=sys.stderr)
            break

        cols = line.split(b"\t")
        if len(cols) < 4:
            output.write(b"Short line:" + line)
            continue
        count += 1

        try:
            start = cols[0]
            start = datetime.datetime.utcfromtimestamp(float(start))
            bytecount = int(cols[1])
            bucket = cols[2].decode()
            source = cols[3].decode()
        except Exception as e:
            print("can't convert row:" + str(e) + ":" + repr(cols))

        try:
            curs.execute(
                "select add_storage ( "
                + """
            %s::text,
            %s::timestamp,
            %s::bigint,
            %s::text)""",
                (bucket, start, bytecount, source),
            )

        except Exception as e:
            db.rollback()
            print("can't insert storage:" + str(e))

        db.commit()
    output.write(b"called stored procedure %d times\n" % count)
    print(f"called stored procedure {count} times\n")
    # curs.execute('ANALYZE')
    #    curs.execute("CLUSTER")

    return


def sess_start_tsv(environ, proc, start_response, output):
    headers = []
    headers.append(("Content-Type", "text/plain"))

    if environ["REQUEST_METHOD"].upper() != "POST":
        output.write(b"Only POST supported\n")
        dump_headers(environ, start_response, output)
        return

    start_response("200 OK", headers)

    output.write(b"\nOutput\n------\n")
    request_len = int(environ.get("CONTENT_LENGTH", 0))
    output.write(b"Got %d input bytes\n" % request_len)

    wsgi_input = environ["wsgi.input"]
    # foo=input.read()
    # print("Got " + repr(foo), file=sys.stderr)
    # print("Got " + str(len(lines)),file=sys.stderr)
    # print("lines" + repr(lines),file=sys.stderr)
    count = 0
    try:
        db = psycopg2.connect(
            "dbname='grafana' user='grafana' host='localhost' password='grafana'"
        )
        curs = db.cursor()
    except Exception as e:
        print("can't connect to the database: " + str(e))

    # lines=input.readlines()
    # output.write(b'\n'.join(lines))
    r = wsgi_input.read()
    lines = r.split(b"\n")
    output.write(b"Got %d lines\n" % len(lines))
    for line in lines:
        if line is None:
            print("Done", file=sys.stderr)
            break
        # print("line is " + repr(line), file=sys.stderr)
        cols = line.split(b"\t")
        if len(cols) < 9:
            output.write(b"Short line:" + line)
            continue
        # output.write(b'\n#%d Cols are:' % count)
        # output.write(b'%d cols ' % len(cols))
        # output.write(b','.join(cols))
        count += 1

        if len(cols) == 9:
            source = "sra prod"
        else:
            source = cols[9].decode()

        try:
            ip = cols[0].decode()
            acc = cols[1].decode()
            agent = cols[2].decode()
            res = int(cols[3])
            domain = cols[4].decode()
            start = cols[5]
            start = datetime.datetime.utcfromtimestamp(float(start))
            end = cols[6]
            end = datetime.datetime.utcfromtimestamp(float(end))
            bytecount = int(cols[7])
            cnt = int(cols[8])
        except Exception as e:
            print("can't convert row:" + str(e) + ":" + repr(cols))

        if ":" in ip:
            ipint = int(ipaddress.IPv6Address(ip))
        else:
            ipint = int(ipaddress.IPv4Address(ip))
        #        with db.cursor() as curs:
        try:
            # Don't care if a few records are lost during server crash, DB will
            # not be corrupted.
            curs.execute("SET synchronous_commit = off")
            curs.execute(
                "select %s  ( " % proc
                + """
            %s::text,
            %s::decimal(39,0),
            %s::text,
            %s::text,
            %s::text,
            %s::integer,
            %s::timestamp,
            %s::timestamp,
            %s::bigint,
            %s::integer,
            %s::text)""",
                (
                    ip,
                    ipint,
                    acc,
                    agent,
                    domain,
                    res,
                    start,
                    end,
                    bytecount,
                    cnt,
                    source,
                ),
            )

        except Exception as e:
            db.rollback()
            print("can't insert %s %s %s" % (proc, start, str(e)))
            print("bad line was " + repr(line))

        db.commit()
    output.write(b"called stored procedure %d times\n" % count)
    print("called stored procedure %d times\n" % count)
    # curs.execute('ANALYZE')
    #    curs.execute("CLUSTER")


def dump_headers(environ, start_response, output):
    headers = []
    headers.append(("Content-Type", "text/plain"))

    start_response("500 Bad", headers)

    keys = environ.keys()
    for key in keys:
        x = "%s: %s\n" % (key, repr(environ[key]))
        output.write(x.encode())
    output.write(b"\n")

    wsgi_input = environ["wsgi.input"]
    lines = wsgi_input.readlines()
    #        inf=input.read()
    output.write(b"Got %d lines\n" % len(lines))
    output.write(b"\n".join(lines))

    return
