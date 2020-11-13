#! /usr/bin/env python3

import logging, logging.handlers

def setup_http_log( host, port ) :
    host_str = '{0}:{1}'.format( host, port )
    handler = logging.handlers.HTTPHandler( host_str, '/', method='GET',
        secure = False )
    logging.getLogger().addHandler( handler )

def setup_buffer_file_log( location : str ):
    # use TimedRotatingFileHandler
    pass