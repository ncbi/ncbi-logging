#! /usr/bin/env python3

import argparse, subprocess, json

def run( args ) :
    d = { "msg" : args.msg }
    message = json.dumps( d )
    print( message )
    proc = subprocess.Popen( [ '/usr/bin/bq', 'insert', args.table ],
        stdin=subprocess.PIPE )
    proc.communicate( input=message.encode( 'utf-8' ), timeout=15 )

if __name__ == "__main__" :
    parser = argparse.ArgumentParser( description = "example log push to google" )
    parser.add_argument( '--msg', '-m', metavar='message', type=str,
        dest='msg', default = 'hallo', help='send message to google' )
    parser.add_argument( '--table', '-t', metavar='table', type=str,
        dest='table', default = 'strides_analytics.test_python_logging',
        help='table name' )
    run( parser.parse_args() )
