#! /usr/bin/env python3

import argparse, logging, logging.handlers

def run( args ) :
    host_str = '{0}:{1}'.format( args.host, args.port )
    handler = logging.handlers.HTTPHandler( host_str, '/', method='GET',
        secure = False )
    logger = logging.getLogger( 'simple ex' )
    fmt = logging.Formatter( '%(asctime) %(message)' )
    handler.setFormatter( fmt )
    logger.addHandler( handler )
    logger.error( 'here we go' )

if __name__ == "__main__" :
    parser = argparse.ArgumentParser( description = "example log sender" )
    parser.add_argument( '--host', '-s', metavar='ip-addr', type=str,
        dest='host', default = '127.0.0.1', help='send to this server' )
    parser.add_argument( '--port', '-p', metavar='number', type=int,
        dest='port', default = 11000, help='send to this port' )
    run( parser.parse_args() )
