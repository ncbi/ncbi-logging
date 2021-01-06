#! /usr/bin/env python3

import unittest
import random
import logging
import requests
import sys
from google.cloud import bigquery
from ncbi_logging_api import setup_http_log

# Construct a BigQuery client object.
client = bigquery.Client()
host="127.0.0.1"
port="11000"
# plug in the logger
setup_http_log( host, port )
logging_ready = False
try:
    response = requests.head( 'http://{0}:{1}'.format( host, port ) )
    logging_ready = ( response.status_code == 200 )
except Exception as e:
    pass

class Test_http_logging( unittest.TestCase ) :
    table = "strides_analytics.application_logging"

    def setUp(self):
        self.msg = "logging API test {0}".format(random.randint(0, 999999))
        query = "delete {0} where msg = '{1}'".format(self.table, self.msg)
        client.query( query )

    def tearDown(self):
        pass

    def test_Msg_Level(self):
        # log the message
        try :
            logging.error( self.msg.format() )
        except :
            print( "could not send to to http-logging server" )
            assert False

        # retrieve message from BQ
        query = "select * from {0} where msg = '{1}'".format(self.table, self.msg)
        rows = client.query(query).result()

        assert 1 == rows.total_rows
        row = next(iter(rows))
        assert self.msg == row["msg"]
        assert "ERROR" == row["levelname"]

if __name__ == "__main__":
    if logging_ready :
        unittest.main() # run all tests
    else :
        print( "logging server not reachable at {0}:{1}".format( host, port ) )
        sys.exit( 1 )
