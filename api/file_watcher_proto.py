#!/usr/bin/env python3

import argparse, sys, os, glob
import grpc, logmon_pb2, logmon_pb2_grpc

def handle( args, stub, filename ) -> bool :
    lines = None
    with open( filename, 'r' ) as f :
        lines = f.readlines()
    if not lines :
        return False
    if len( lines ) < 1 :
        return True
    for line in lines :
        try :
            b = bytes.fromhex( line.rstrip() )
            msg = logmon_pb2.LogMessage()
            msg.ParseFromString( b )
            print( msg )
            reply = stub.SendLogMessage( msg )
            # what happens if it fails...
        except Exception as e:
            print( e )
    #remove the file if all send
    return True
    
def run( args ) -> bool :
    if None == args.location :
        print( "pattern is missing" )
        return False
    if not os.path.isdir( os.path.dirname( args.location ) ):
        return False
    addr = '{}:{}'.format( args.host, args.port )
    files = glob.glob( args.location + "*" )
    files.sort()
    with grpc.insecure_channel( addr ) as channel :
        stub = logmon_pb2_grpc.MessageTransportStub( channel ) #TBD
        for fn in files :
            #this excludes the file that is currently written to
            if fn != args.location :
                if not handle( args, stub, fn ) :
                    return False
    return True

if __name__ == "__main__":
    #example location: /path/to/logs/logs
    parser = argparse.ArgumentParser( description = "file watcher" )
    parser.add_argument( '--pattern', '-a', metavar='path', type=str,
        dest='location', default = None, help='files to watch' )
    parser.add_argument( '--host', '-o', metavar='dns-name', type=str,
        dest='host', default = 'localhost', help='dns-name of machine to send logs to' )
    parser.add_argument( '--port', '-p', metavar='portnumber', type=int,
        dest='port', default = 10001, help='port on dns-name' )

    res = run( parser.parse_args() )
    if not res :
        sys.exit( 1 )


