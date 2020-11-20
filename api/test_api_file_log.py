#! /usr/bin/env python3

import unittest
import random
import logging
import os, glob, json, time, shutil
import ncbi_logging_api

class Test_file_logging( unittest.TestCase ) :

    def setUp( self ) :
        self.location = os.path.join( os.getcwd(), 'tmp', unittest.TestCase.id( self ) )
        shutil.rmtree( self.location, ignore_errors=True )
        os.makedirs( self.location, exist_ok=True )
        self.filename = os.path.join( self.location, 'log' )
        self.msg = "logging API test {0}".format(random.randint(0, 999999))

    def tearDown( self ) :
        shutil.rmtree( self.location, ignore_errors=True )

    def verify_line( self, line : str, d ) :
        j = json.loads( line.strip() )
        for k, v in d.items() :
            self.assertEqual( j[ k ], v, "for key:'{}'".format( k ) )

    def verify_file( self, fn : str, d ) :
        with open( fn ) as f :
            line_nr = 0
            for l in f :
                if line_nr == 0 :
                    self.verify_line( l, d )
                line_nr += 1
            assert 1 == line_nr

    # def test_Msg_Level( self ) :
    #     ncbi_logging_api.setup_buffer_file_log( self.filename )
    #     logging.getLogger().setLevel( logging.DEBUG )
    #     logging.error( self.msg, { 'a' : 'b' } )
    #     logging.info( self.msg, { 'c' : 'd' } )
    #     assert 1 == len( glob.glob( os.path.join( self.location, "*" ) ) )
    #     with open( self.filename ) as f :
    #         line_nr = 0
    #         for l in f :
    #             if line_nr == 0 :
    #                 self.verify_line( l, { 'a':"{'a': 'b'}", 'l':"ERROR", 'm':self.msg } )
    #             elif line_nr == 1 :
    #                 self.verify_line( l, { 'a':"{'c': 'd'}", 'l':"INFO", 'm':self.msg } )
    #             line_nr += 1
    #         assert 2 == line_nr

    # def test_Msg_Format( self ) :
    #     ncbi_logging_api.setup_buffer_file_log( self.filename )
    #     logging.getLogger().setLevel( logging.DEBUG )
    #     logging.error( self.msg + "%(c)s", { 'a' : 'b', 'c':'e' } )
    #     logging.info( self.msg + "%(c)s", { 'c' : 'd' } )
    #     logging.info( self.msg + "%d %d %d", 1, 2, 3 )
    #     assert 1 == len( glob.glob( os.path.join( self.location, "*" ) ) )
    #     with open( self.filename ) as f :
    #         line_nr = 0
    #         for l in f :
    #             if line_nr == 0 :
    #                 self.verify_line( l, { 'a':"{'a': 'b', 'c': 'e'}", 'l':"ERROR", 'm':self.msg + 'e' } )
    #             elif line_nr == 1 :
    #                 self.verify_line( l, { 'a':"{'c': 'd'}", 'l':"INFO", 'm':self.msg + 'd' } )
    #             elif line_nr == 2 :
    #                 self.verify_line( l, { 'a':'(1, 2, 3)', 'l':"INFO", 'm':self.msg + '1 2 3' } )
    #             line_nr += 1

    def test_Msg_Extra( self ) :
        ncbi_logging_api.setup_buffer_file_log( self.filename )
        #logging.error( self.msg, extra = { 'z' : 'y' } )
        logging.error( self.msg )
        with open( self.filename ) as f :
            print( f.readline() )

    # def test_Rotation( self ) :
    #     ncbi_logging_api.setup_buffer_file_log( self.filename, arg_when='S', arg_interval=1 )
    #     logging.getLogger().setLevel( logging.DEBUG )
    #     logging.error( self.msg, { 'a' : 'b' } )
    #     time.sleep( 1.5 )
    #     logging.info( self.msg, { 'c' : 'd' } )
    #     files = glob.glob( os.path.join( self.location, "*" ) )
    #     assert 2 == len( files )
    #     for fn in files :
    #         if fn != self.filename :
    #             self.verify_file( fn, { 'a':"{'a': 'b'}", 'l':"ERROR", 'm':self.msg } )
    #         else :
    #             self.verify_file( fn, { 'a':"{'c': 'd'}", 'l':"INFO", 'm':self.msg } )

if __name__ == "__main__":
    unittest.main() # run all tests
