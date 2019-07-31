#!/usr/bin/env python3.7

from google.cloud import bigquery

CLIENT = bigquery.Client(project="strides-sra-hackathon-data")

for job in CLIENT.list_jobs(all_users=True):
    # print job
    # print >>sys.stderr, dir(job)
    try:
        # print job.job_id
        # print job.job_type
        if job.job_type == "query" and job.total_bytes_billed:
            fulljob = CLIENT.get_job(job.job_id)
            # print "type fulljob", type(fulljob)
            # print "dir fulljob", dir(fulljob)
            tables = ""
            if fulljob.referenced_tables:
                # print "fj rt", fulljob.referenced_tables
                tables = fulljob.referenced_tables[0].table_id
            username = job.user_email.split("@")[0]
            start_ts = job.started
            #            print (type(start_ts))
            #            start_ts=start_ts.replace('+00:00','',1)
            #            start_ts=datetime.datetime.strptime(start_ts,
            #                    '%Y-%m-%d %H:%M:%S.%f')
            # start_ts=start_ts.strftime("%s")
            start_ts = start_ts.strftime("%Y-%m-%d %H:%M:%S.%f")
            bytecount = job.total_bytes_billed
            # print '\t'.join((str(start_ts),
            #                username,
            #                str(bytecount),
            #                str(tables)))
            print("insert into bigqueries")
            print("(start_ts, bytes, tables, source, username, created, updated)")
            print("values (")
            print("'%s'," % start_ts)
            print("%d," % bytecount)
            print("'%s'," % tables)
            print("'sra hackathon',")
            print("'%s'," % username)
            print("now(),")
            print("now()")
            print(") on conflict do nothing;")
    #            print "plan", fulljob.query_plan
    #            plan=fulljob.query_plan
    #            print "dir queryplan", dir(plan)
    #            for qp in plan:
    #                print "qp is", qp
    #                print "qp has", dir(qp)
    #                if qp.referenced_tables:
    #                    print "rt", qp.referenced_tables
    # print "tables", fulljob.table_
    except AttributeError as e:
        print(e)
        continue
