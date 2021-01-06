#! /usr/bin/env python3

import argparse, sys, os, glob, logging, http.client
from urllib.parse import urlparse, urlunparse

# False if there was at least one error opening or transmitting
#   (some files may have been processed)
# True if all files have been processed or there are none

#Todo: add a batch-size to make multiple POST-requests to the server for
#each file if the file is bigger than batch-size
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
    files.sort()
    for fn in files :
        if fn != basename :
            try:
                if conn == None :
                    conn = client.HTTPConnection( o.hostname, o.port )
            except Exception as e:
                # where this log message goes is up to the environment
                logging.error( "perform_file_watching client.HTTPConnection failed (%s)", str(e) )
                return False

            try:

                with open( fn ) as f:
                    good = True
                    try:
                        conn.request('POST', o.path, "".join(f.readlines()) )
                    except Exception as e:
                        logging.error( "perform_file_watching conn.request(%s %s %s) failed (%s)", o.hostname, o.port, o.path, str(e) )
                        good = False

                    if good :
                        try:
                            if conn != None :
                                resp = conn.getresponse()
                                if 200 == resp.status:
                                    os.remove( fn )
                                else:
                                    success = False
                        except Exception as e:
                            logging.error( "perform_file_watching conn.getresponse() failed (%s)", str(e) )
                            good = False

                    if not good :
                        conn.close()
                        conn = None # will reopen on the next iteration
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

    res = perform_file_watching( args.location, args.url, http.client )
    if not res :
        sys.exit( 1 )
