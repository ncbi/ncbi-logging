#! /usr/bin/env python3

import argparse, logging, logging.handlers

def setup_http_log( host, port ) :
    host_str = '{0}:{1}'.format( host, port )
    handler = logging.handlers.HTTPHandler( host_str, '/', method='GET',
        secure = False )
    logging.getLogger().addHandler( handler )

def run( args ) :
    setup_http_log( args.host, args.port )
    logging.error( args.msg )

if __name__ == "__main__" :
    parser = argparse.ArgumentParser( description = "example log sender" )
    parser.add_argument( '--host', '-s', metavar='ip-addr', type=str,
        dest='host', default = '127.0.0.1', help='send to this server' )
    parser.add_argument( '--port', '-p', metavar='number', type=int,
        dest='port', default = 11000, help='send to this port' )
    parser.add_argument( '--message', '-m', metavar='message', type=str,
        dest='msg', default = "dflt message", help='the message sent' )
    run( parser.parse_args() )
