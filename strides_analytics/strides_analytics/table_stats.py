#!/usr/bin/env python3

import json
import os
import psycopg2
import sys

try:
    conn = psycopg2.connect(
        "dbname='strides_analytics' user='sa_prod_read' host='intprod11'"
    )
except e:
    print("I am unable to connect to the database" + e)

tables = {}
result = {}
with conn:
    with conn.cursor() as curs:
        curs.execute(
            """
            SELECT tablename FROM pg_catalog.pg_tables
            WHERE schemaname != 'pg_catalog'
            AND schemaname != 'information_schema'"""
        )
        for record in curs:
            tables[record[0]] = {}

    for table in tables:
        with conn.cursor() as curs:
            curs.execute(
                """
            SELECT column_name, data_type FROM information_schema.columns WHERE
            table_name   = %s """,
                (table,),
            )
            for record in curs:
                tables[table][record[0]] = record[1]

    for table in tables:
        result[table] = {}
        cols = tables[table]
        for col in cols:
            tp = cols[col]

            print("%s.%s" % (table, col), file=sys.stderr)
            with conn.cursor() as curs:
                q = "select min(%s), max(%s), count(distinct %s) from %s" % (
                    col,
                    col,
                    col,
                    table,
                )
                curs.execute(q)
                for record in curs:
                    result[table][col] = {
                        "min": str(record[0]),
                        "max": str(record[1]),
                        "count": record[2],
                    }

print(json.dumps(result, sort_keys=True, indent=4))
#    with open("out.json", "w") as out:
#        out.write(json.dumps(result))
