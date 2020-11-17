#! /usr/bin/env python3

import unittest, http.client
import file_watcher

class Test_file_watching( unittest.TestCase ) :
    def setUp( self ) :
        pass

    def tearDown( self ) :
        pass

    def test_one( self ) :
        self.assertFalse( file_watcher.perform_file_watching( None, 'somewhere', http.client ) )
        self.assertFalse( file_watcher.perform_file_watching( 'somewhere', None, http.client ) )
        self.assertFalse( file_watcher.perform_file_watching( 'somewhere', 'somewhere', None ) )

if __name__ == "__main__":
    unittest.main() # run all tests

