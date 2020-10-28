#! /usr/bin/env python3

import unittest
import splunkreq

class Test_file2string(unittest.TestCase):
    def setUp(self):
        pass

    def tearDown(self):
        pass

    def testEmpty(self):
        try:
            assert splunkreq.file2string( "" ) == ""
        except:
            return
        self.fail ("no exception")

    def testDoesNotExist(self):
        try:
            assert splunkreq.file2string( "i am not there" ) == ""
        except:
            return
        self.fail ("no exception")

    def testDoesExist(self):
        res = splunkreq.file2string( "test_data.txt" )
        assert res == "data\n"


class Test_argsParse(unittest.TestCase):
    def setUp(self):
        pass

    def tearDown(self):
        pass

    def testDefaults(self):
        args = splunkreq.parseArgs( [] )
        assert args.bearer == ""
        assert args.earliest == "-1m"
        assert args.latest == ""
        assert args.timeout == 100

    def testLongArgs(self):
        args = splunkreq.parseArgs( ['--bearer', 'bearer.txt', '--earliest=-5m', '--latest', '10/27/2018:00:00:00', '--timeout', '1'] )
        assert args.bearer == "bearer.txt"
        assert args.earliest == '-5m'
        assert args.latest == '10/27/2018:00:00:00'
        assert args.timeout == 1

    def testShortArgs(self):
        args = splunkreq.parseArgs( ['-b', 'bearer.txt', '-e', '10/27/2018:00:00:00', '-l=-1h', '-t', '20'] )
        assert args.bearer == "bearer.txt"
        assert args.earliest == "10/27/2018:00:00:00"
        assert args.latest == '-1h'
        assert args.timeout == 20


if __name__ == "__main__":
    unittest.main() # run all tests