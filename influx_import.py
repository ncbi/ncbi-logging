#!/usr/bin/env python3
# wget https://dl.influxdata.com/influxdb/releases/influxdb_1.7.10_amd64.deb
# sudo dpkg -i influxdb_1.7.10_amd64.deb
# sudo apt-get update && sudo apt-get install influxdb
# sudo service influxdb start
# influx
# create database strides
# use strides

# line protocol is: <measurement>[,<tag-key>=<tag-value>...]
# <field-key>=<field-value>[,<field2-key>=<field2-value>...]
# [unix-nano-timestamp]

# zcat *gz | head -n 1000000 | ./influx_import.py | split -l 5000 - inserts_
# for x in inserts_*; do curl -i -XPOST  'http://localhost:8086/write?db=strides' --data-binary @$x; done

# tags are indexed, fields are not
import sys
from datetime import date
import datetime
import time

seq = time.time()
for line in sys.stdin:
    line = line.strip()
    line = line.replace('"', "")
    fields = line.split(" ")
    #    print(str(fields))
    ip = fields[0]
    host = fields[5]
    # host = host.replace('"', "")
    cmd = fields[6]
    # cmd = cmd.replace('"', "")
    url = fields[7]

    measurement = "nginx"
    tags = "ip=%s,host=%s" % (ip, host)
    fields = 'cmds="%s",url="%s"' % (cmd, url)
    print("%s,%s %s %d" % (measurement, tags, fields, seq * 1000000000))
    seq += 1.0
#    print(f"{measurement},f{tags} f{fields}")
