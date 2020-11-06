#! /usr/bin/env python3

import argparse, http.server, socketserver, json, subprocess
from urllib.parse import urlparse, parse_qs

class my_request_handler( http.server.SimpleHTTPRequestHandler ) :
    def do_GET( self ) :
        host, port = self.client_address
        #print( 'path={0}'.format( self.path ) )
        o = urlparse( self.path )
        d = parse_qs( o.query )
        d2 = {}
        for k, v in d.items() :
            d2[ k ] = v[ 0 ]
        d2[ 'host' ] = host
        d2[ 'port' ] = port
        self.send_to_google( d2 )

        self.path = 'index.html'
        return http.server.SimpleHTTPRequestHandler.do_GET( self )

    def send_to_google( self, d ) :
        message = json.dumps( d )
        print( message )
        table = 'strides_analytics.application_logging'
        proc = subprocess.Popen( [ '/usr/bin/bq', 'insert', table ],
            stdin=subprocess.PIPE )
        proc.communicate( input=message.encode( 'utf-8' ), timeout=5 )


def run( args ) :
    handler = my_request_handler
    with socketserver.TCPServer( ("", args.port ), handler ) as httpd:
        print( 'listening on port {}'.format( args.port) )
        try :
            httpd.serve_forever()
        except :
            httpd.shutdown()

if __name__ == "__main__" :
    parser = argparse.ArgumentParser( description = "example log receiver" )
    parser.add_argument( '--port', '-p', metavar='number', type=int,
        dest='port', default = 11000, help='listen on this port' )
    run( parser.parse_args() )
