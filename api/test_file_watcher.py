#! /usr/bin/env python3

import unittest, unittest.mock

import os, shutil, logging, signal, time
import http.client, http.server, socketserver

import file_watcher

from unittest.mock import Mock

def createFile( filename : str, content : str ) -> str :
    with open( filename, "w") as f:
        f.write( content )
    return filename

def makeMockClient( status : int = 200 ) :
    connection = Mock()
    client = Mock()
    client.HTTPConnection = Mock( return_value = connection )
    connection.request = Mock()
    response = Mock()
    response.status = status
    connection.getresponse = Mock( return_value = response )
    return connection, client

class Nothing:
    def __init__ ( self ):
        pass

class MockedResponse:
    def __init__( self, stat ):
        self.status = stat

class my_request_handler( http.server.BaseHTTPRequestHandler ) :
    datafile : str = ""

    def do_POST( self ) :
        #host, port = self.client_address
        #print( 'host={0} port ={1} path={2}'.format( host, port, self.path ) )

        #save the payload in a file for verification
        length = int(self.headers['Content-Length'])
        post_data = self.rfile.read(length).decode('utf-8')
        createFile( my_request_handler.datafile, post_data )
        self.send_response( 200 )
        self.end_headers()

    def log_message( self, format, *args ) :
        pass

class Test_file_watching( unittest.TestCase ) :
    def setUp( self ) :
        self.location = os.path.join( os.getcwd(), 'tmp', unittest.TestCase.id( self ) )
        shutil.rmtree( self.location, ignore_errors=True )
        os.makedirs( self.location, exist_ok=True )
        self.log = os.path.join( self.location, "log" )

        self.logger = logging.getLogger()
        self.level = self.logger.getEffectiveLevel()

    def tearDown( self ) :
        self.logger.setLevel(self.level)
        shutil.rmtree( self.location, ignore_errors=True )

    def test_one( self ) :
        self.assertFalse( file_watcher.perform_file_watching( None, 'somewhere', http.client ) )
        self.assertFalse( file_watcher.perform_file_watching( 'somewhere', None, http.client ) )
        self.assertFalse( file_watcher.perform_file_watching( 'somewhere', 'somewhere', None ) )

    def test_bad_location( self ) :
        # the full basename does not have to exist, but the directory part of it should
        self.assertFalse( file_watcher.perform_file_watching( '/i do not exist/basename', 'somewhere', http.client ) )

    def test_empty_dir( self ) :
        # 'Nothing' would blow up if called in any way
        self.assertTrue( file_watcher.perform_file_watching( self.log, 'somewhere', Nothing ) )

    def test_unfinished_log( self ) :
        # log withtout timestamp should not be touched
        with open( self.log,"w") as f:
            f.write("blah")
        self.assertTrue( file_watcher.perform_file_watching( self.log, 'somewhere', Nothing ) )

    def test_transmit( self ) :
        filename = createFile( self.log + ".suffix", "blah" )

        connection, client = makeMockClient()

        self.assertTrue( file_watcher.perform_file_watching( self.log, 'http://a.b.c:8899/endpoint/dir/file?k1=v1&k2=v2', client ) )

        self.assertTrue( client.HTTPConnection.called )
        self.assertEqual( 1, client.HTTPConnection.call_count )
        self.assertEqual( ('a.b.c', 8899), client.HTTPConnection.call_args[0] )

        self.assertEqual( 1, connection.request.call_count )
        self.assertEqual( 'POST', connection.request.call_args[0][0] )
        self.assertEqual( '/endpoint/dir/file', connection.request.call_args[0][1] )
        self.assertEqual( "blah", connection.request.call_args[0][2] )

        self.assertFalse( os.path.exists(filename) )

    def test_file_open_fails( self ) :
        filename = createFile( self.log + ".suffix", "blah" )
        os.chmod(filename, 2)

        _, client = makeMockClient()

        # temporarily block ERROR messages from going to stderr
        self.logger.setLevel( logging.CRITICAL )

        self.assertFalse( file_watcher.perform_file_watching( self.log, 'http://a.b.c:8899', client ) )

    def test_file_open_fails_for_one_of_many( self ) :
        filename1 = createFile( self.log + ".1", "blah1" )
        filename2 = createFile( self.log + ".2", "blah2" )
        filename3 = createFile( self.log + ".3", "blah3" )
        # one of the 3 files will not open
        os.chmod(filename2, 2)

        _, client = makeMockClient()

        # temporarily block ERROR messages from going to stderr
        self.logger.setLevel( logging.CRITICAL )

        self.assertFalse( file_watcher.perform_file_watching( self.log, 'http://a.b.c:8899', client ) )

        # files 1 and 3 have been processed and removed, the 2 remains on disk
        self.assertFalse( os.path.exists( filename1 ) )
        self.assertTrue(  os.path.exists( filename2 ) )
        self.assertFalse( os.path.exists( filename3 ) )

    def test_http_connections_fails( self ) :
        filename = createFile( self.log + ".suffix", "blah" )
        self.logger.setLevel( logging.CRITICAL )
        self.assertFalse( file_watcher.perform_file_watching( self.log, 'http://a.b.c:port', http.client ) )
        self.assertTrue( os.path.exists(filename) )

    def test_request_fails( self ) :
        filename = createFile( self.log + ".suffix", "blah" )

        _, client = makeMockClient( 500 )

        self.assertFalse( file_watcher.perform_file_watching( self.log, 'http://a.b.c:8899/endpoint/dir/file?k1=v1&k2=v2', client ) )

        self.assertTrue( os.path.exists(filename) )

    def test_request_fails_for_one_of_many( self ) :
        filename1 = createFile( self.log + ".1", "blah1" )
        filename2 = createFile( self.log + ".2", "blah2" )
        filename3 = createFile( self.log + ".3", "blah3" )

        connection = Mock()
        client = Mock()
        client.HTTPConnection = Mock( return_value = connection )
        connection.request = Mock()
        response = iter([MockedResponse(200), MockedResponse(500), MockedResponse(200)])
        connection.getresponse = Mock( side_effect = response )

        self.assertFalse( file_watcher.perform_file_watching( self.log, 'http://a.b.c:8899', client ) )

        # files 1 and 3 have been processed and removed, the 2 remains on disk
        self.assertFalse( os.path.exists( filename1 ) )
        self.assertTrue(  os.path.exists( filename2 ) )
        self.assertFalse( os.path.exists( filename3 ) )

    def test_request_throws( self ) :
        filename = createFile( self.log + ".suffix", "blah" )

        connection = Mock()
        client = Mock()
        client.HTTPConnection = Mock( return_value = connection )
        connection.request = Mock( side_effect = Exception )

        # temporarily block ERROR messages from going to stderr
        self.logger.setLevel( logging.CRITICAL )

        self.assertFalse( file_watcher.perform_file_watching( self.log, 'http://a.b.c:8899/endpoint/dir/file?k1=v1&k2=v2', client ) )

        self.assertTrue( os.path.exists(filename) )

    def test_on_http_server( self ):
        port = 12000
        datafile = os.path.join( self.location, ".post" )
        newpid = os.fork()
        if newpid == 0:
            my_request_handler.datafile = datafile
            with socketserver.TCPServer( ("", port ), my_request_handler ) as httpd:
                try :
                    httpd.serve_forever()
                except :
                    httpd.shutdown()
            exit(0)

        try:
            time.sleep(1)

            filename = createFile( self.log + ".suffix", "blah1\nblah2" )
            self.assertTrue( file_watcher.perform_file_watching(
                self.log,
                'http://127.0.0.1:{0}/'.format(port),
                http.client ) )

            self.assertFalse( os.path.exists(filename) )

            #TODO: verify that server received the file contents
            with open( datafile, "r" ) as f:
                lines = f.readlines()

            self.assertEqual( 2, len( lines ) )
            self.assertEqual( "blah1", lines[0].strip()  )
            self.assertEqual( "blah2", lines[1].strip()  )

        finally:
            os.kill(newpid, signal.SIGKILL)
            pass

if __name__ == "__main__":
    unittest.main() # run all tests

