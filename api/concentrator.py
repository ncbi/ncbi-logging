#!/usr/bin/env python3

import argparse, json
import grpc, logmon_pb2 , logmon_pb2_grpc
from concurrent import futures

server = grpc.server( futures.ThreadPoolExecutor( max_workers=10 ) )

class MessageTransportServicer( logmon_pb2_grpc.MessageTransportServicer ) :
    def SendLogMessage( self, msg, context ) :
        d = { 'created': msg.created, 'filename': msg.filename, 'funcName': msg.funcName,
            'levelno': msg.levelno, 'lineno': msg.lineno, 'message': msg.message,
            'module': msg.module, 'name': msg.name, 'pathname': msg.pathname,
            'process': msg.process, 'processName': msg.processName,
            'relativeCreated': msg.relativeCreated, 'thread': msg.thread,
            'threadName' : msg.threadName }
        # todo add the extra-data ( repeated in msg )
        json_object = json.dumps( d )
        print( json_object )

        res = logmon_pb2.TransportResult()
        res . status = 200
        return res

def run( args ) :
    logmon_pb2_grpc.add_MessageTransportServicer_to_server(
        MessageTransportServicer(), server )
    server.add_insecure_port( "[::]:{}".format( args.port ) )
    server.start()
    server.wait_for_termination()

if __name__ == "__main__" :
    parser = argparse.ArgumentParser( description = "logmon protobuf concentrator" )
    parser.add_argument( '--port', '-p', metavar='number', type=int,
        dest='port', default = 10001, help='send to this port' )
    run( parser.parse_args() )
