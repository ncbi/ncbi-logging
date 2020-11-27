#!/usr/bin/env python3

import argparse
import experiment_pb2
import experiment_pb2_grpc
import grpc

def messages( filename : str ) :
    with open( filename, 'r' ) as f :
        for line in f :
            msg = experiment_pb2.LogMessage()
            msg.message = line.strip()
            yield msg

def run( args ) :
    addr = '{}:{}'.format( args.host, args.port )
    with grpc.insecure_channel( addr ) as channel :
        stub = experiment_pb2_grpc.MessageTransportStub( channel )
        try :
            if args.stop > 0 :
                msg = experiment_pb2.StopMessage()
                msg.txt = args.msg
                reply = stub.SendStopMessage( msg )
            elif args.file :
                reply = stub.SendLogMessageStream( messages( args.file ) )
            else :
                msg = experiment_pb2.LogMessage()
                msg.message = args.msg
                reply = stub.SendLogMessage( msg )
            print( reply )

        except Exception as e :
            print( e )

if __name__ == "__main__" :
    parser = argparse.ArgumentParser( description = "example sender" )
    parser.add_argument( '--message', '-m', metavar='message', type=str,
        dest='msg', default = "dflt message", help='the message sent' )
    parser.add_argument( '--host', '-s', metavar='ip-addr', type=str,
        dest='host', default = '127.0.0.1', help='send to this server' )
    parser.add_argument( '--port', '-p', metavar='number', type=int,
        dest='port', default = 11000, help='send to this port' )
    parser.add_argument( '--stop', '-t', metavar='number', type=int,
        dest='stop', default = 0, help='stop the server' )
    parser.add_argument( '--file', '-f', metavar='filename', type=str,
        dest='file', default = None, help='send a file of messages' )

    run( parser.parse_args() )
