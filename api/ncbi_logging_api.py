#! /usr/bin/env python3

import logging, logging.handlers
import os, sys

prev_handler = None

def helper_set_handler( handler ) :
    logger = logging.getLogger()
    this = sys.modules[__name__]
    if this.prev_handler != None :
        this.prev_handler.close()
        logger.removeHandler( this.prev_handler )
    logger.addHandler( handler )
    this.prev_handler = handler

def setup_http_log( host, port ) :
    host_str = '{0}:{1}'.format( host, port )
    handler = logging.handlers.HTTPHandler( host_str, '/', method='GET',
        secure = False )
    helper_set_handler( handler )

def setup_buffer_file_log( filename : str, arg_when = 'M', arg_interval = 1 ):
    fmt = logging.Formatter( 
        '{ "t":"%(asctime)-15s", "l":"%(levelname)s", "m":"%(message)s", "x":"%(extra)s" }' )
    handler = logging.handlers.TimedRotatingFileHandler( filename,
        when=arg_when, interval=arg_interval )
    handler.setFormatter( fmt )
    helper_set_handler( handler )
