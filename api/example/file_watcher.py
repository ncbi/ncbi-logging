#! /usr/bin/env python3

import argparse, sys

def perform_file_watching( location : str, concentrator_url : str, client ) -> bool :
    if None == location :
        return False
    if None == concentrator_url :
        return False
    if None == client :
        return False
    return True

if __name__ == "__main__":
    parser = argparse.ArgumentParser( description = "file watcher" )
    parser.add_argument( '--loc', '-l', metavar='path', type=str,
        dest='location', default = None, help='directory to watch' )
    parser.add_argument( '--url', '-u', metavar='url', type=str,
        dest='url', default = None, help='url to send logs to' )
    args = parser.parse_args()

    res = perform_file_watching( args.location, arg.url, http.client )
    if not res :
        sys.exit( 1 )
