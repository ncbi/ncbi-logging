#!/usr/bin/env python3

import argparse
import experiment_pb2
import experiment_pb2_grpc
import grpc
from concurrent import futures

server = grpc.server( futures.ThreadPoolExecutor( max_workers=10 ) )

class MessageTransportServicer( experiment_pb2_grpc.MessageTransportServicer ) :
    def SendLogMessage( self, request, context ) :
        print( request )
        res = experiment_pb2.TransportResult()
        res . status = 200
        return res

    def SendLogMessageStream( self, request, context ) :
        for m in request :
            print( m )
        res = experiment_pb2.TransportResult()
        res . status = 200
        return res

    def SendStopMessage( self, request, context ) :
        print( request )
        res = experiment_pb2.TransportResult()
        res . status = 200
        server.stop( True )
        return res

def run( args ) :
    experiment_pb2_grpc.add_MessageTransportServicer_to_server(
        MessageTransportServicer(), server )
    server.add_insecure_port( "[::]:{}".format( args.port ) )
    server.start()
    server.wait_for_termination()

if __name__ == "__main__" :
    parser = argparse.ArgumentParser( description = "example sender" )
    parser.add_argument( '--port', '-p', metavar='number', type=int,
        dest='port', default = 11000, help='send to this port' )
    run( parser.parse_args() )
