#! /usr/bin/env python3

import argparse, sys, os, glob, logging
from urllib.parse import urlparse, urlunparse

# False if there was at least one error opening or transmitting
#   (some files may have been processed)
# True if all files have been processed or there are none
def perform_file_watching( basename : str, concentrator_url : str, client ) -> bool :
    if None == basename :
        return False
    if None == concentrator_url :
        return False
    if None == client :
        return False

    if not os.path.isdir( os.path.dirname( basename ) ):
        return False

    o = urlparse( concentrator_url )
    success = True
    conn = None
    files = glob.glob( basename + "*" )
    for fn in files :
        if fn != basename :
            try:
                if conn == None :
                    conn = client.HTTPConnection( urlunparse( (o.scheme, o.netloc, "", "", "", "" ) ) )
            except Exception as e:
                # where this log message goes is up to the environment
                logging.error( "perform_file_watching client.HTTPConnection failed (%s)", str(e) )
                return False

            try:

                with open( fn ) as f:
                    try:
                        conn.request('POST', o.path, f.readlines() )
                    except Exception as e:
                        logging.error( "perform_file_watching conn.request failed (%s)", str(e) )
                        conn.close()
                        conn = None # will reopen on the next iteration
                        success = False

                    if conn != None :
                        resp = conn.getresponse()
                        if 200 == resp.status:
                            os.remove( fn )
                        else:
                            success = False

            except Exception as e:
                logging.error( "perform_file_watching open(%s) failed (%s)", fn, str(e) )
                success = False

    if conn != None :
        conn.close()

    return success

if __name__ == "__main__":
    parser = argparse.ArgumentParser( description = "file watcher" )
    parser.add_argument( '--pattern', '-p', metavar='path', type=str,
        dest='location', default = None, help='files to watch' )
    parser.add_argument( '--url', '-u', metavar='url', type=str,
        dest='url', default = None, help='url to send logs to' )
    args = parser.parse_args()

    res = perform_file_watching( args.location, arg.url, http.client )
    if not res :
        sys.exit( 1 )
