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



if __name__ == "__main__":
    unittest.main() # run all tests