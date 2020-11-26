#! /usr/bin/env python3

import argparse, logging, os, random, time, json
from ncbi_logging_api import setup_buffer_file_log

def str_2_level( s : str ) :
    if "error" == s :
        return logging.ERROR
    elif "debug" == s :
        return logging.DEBUG
    elif "info" == s :
        return logging.INFO
    elif "warn" == s :
        return logging.WARN
    else :
        return logging.NOTSET

def send_msg_with_dict( i, args, d ) :
    #print( d )
    msg = "message #{}".format( i + 1 )
    if "error" == args.severity :
        logging.error( msg, extra = d )
    elif "debug" == args.severity :
        logging.debug( msg, extra = d )
    elif "info" == args.severity :
        logging.info( msg, extra = d )
    elif "warn" == args.severity :
        logging.warning( msg, extra = d )
    else :
        logging.critical( msg, extra = d )

def logloop( args ) :
    d = None
    if args.args :
        d = { "extra" : json.loads( args.args ) }
    for i in range( args.count ) :
        send_msg_with_dict( i, args, d )
        ms_to_wait = random.randint( args.min_wait, args.max_wait )
        s_to_wait = ms_to_wait / 1000.0
        time.sleep( s_to_wait )

def run( args ) :
    head, tail = os.path.split( args.location )
    if not os.path.isdir( head ) :
        os.makedirs( head )
    try :
        setup_buffer_file_log( args.location, args.when, args.interval )
        logging.getLogger().setLevel( str_2_level( args.level ) )
        logloop( args )
    except Exception as e:
        print( 'cannot setup log-handler: {}'.format( e ) )

if __name__ == "__main__":
    parser = argparse.ArgumentParser( description = "application example" )
    parser.add_argument( '--location', '-l', metavar='path', type=str,
        dest='location', default = 'logdir/log', help='where to write log-messages' )
    parser.add_argument( '--count', '-c', metavar='number', type=int,
        dest='count', default = 10, help='how many to send' )
    parser.add_argument( '--min-wait', '-m', metavar='number', type=int,
        dest='min_wait', default = 10, help='minimum wait-time in milli-seconds' )
    parser.add_argument( '--max-wait', '-x', metavar='number', type=int,
        dest='max_wait', default = 200, help='maximum wait-time in milli-seconds' )
    parser.add_argument( '--when', '-w', metavar='selector', type=str,
        dest='when', default = 'M', help='unit for log rotation (M...minutes, S...seconds)' )
    parser.add_argument( '--interval', '-i', metavar='time', type=int,
        dest='interval', default = 1, help='after how many units perform rotation' )
    parser.add_argument( '--args', '-a', metavar='arguments', type=str,
        dest='args', default = None, help='arguments to append to log' )
    parser.add_argument( '--severity', '-s', metavar='selector', type=str,
        dest='severity', default = "error", help='what severity to log' )
    parser.add_argument( '--level', '-e', metavar='selector', type=str,
        dest='level', default = "info", help='minimum level to log' )

    run( parser.parse_args() )
