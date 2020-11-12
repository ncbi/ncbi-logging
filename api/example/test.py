#! /usr/bin/env python3

import unittest
import random
import logging
from google.cloud import bigquery
from ncbi_logging_api import setup_http_log

# Construct a BigQuery client object.
client = bigquery.Client()

class Test_file2string(unittest.TestCase):
    table = "strides_analytics.application_logging"

    def setUp(self):
        # plug in the logger
        host="127.0.0.1"
        port="11000"
        setup_http_log( host, port )
        #

        self.msg = "logging API test {0}".format(random.randint(0, 999999))
        query = "delete {0} where msg = '{1}'".format(self.table, self.msg)
        client.query(query)

        pass

    def tearDown(self):
        pass

    def test_Msg_Level(self):
        # log the message
        logging.error( self.msg.format() )

        # retrieve message from BQ
        query = "select * from {0} where msg = '{1}'".format(self.table, self.msg)
        rows = client.query(query).result()

        assert 1 == rows.total_rows
        row = next(iter(rows))
        assert self.msg == row["msg"]
        assert "ERROR" == row["levelname"]

if __name__ == "__main__":
    unittest.main() # run all tests

