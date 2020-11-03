#! /usr/bin/env python3

import argparse

import sys, http, urllib, xml.dom.minidom, urllib.request, time, json
from xml.dom.minidom import parse

def file2string( filename : str ) -> str :
    with open( filename, "r" ) as f :
        return f . read()

def extract_search_id_from_xml( data : str ) -> str :
    try :
        DOM = xml.dom.minidom.parseString( data )
        coll = DOM.documentElement
        for c in coll.childNodes :
            if type( c ) is xml.dom.minidom.Element :
                if c . tagName == 'sid' :
                    return c . firstChild . data
        return "error 1"
    except Exception as ex:
        return "error"

def extract_status_from_xml( data : str ) -> ( str, str ) :
    status = "UNKNOWN"
    count = ""
    try :
        DOM = xml.dom.minidom.parseString( data )
        coll = DOM.documentElement
        c = coll.getElementsByTagName( "content" )
        if len( c ) > 0 :
            c0 = c[ 0 ]
            a = c0.getElementsByTagName( "s:dict" )
            if len( c ) > 0 :
                a0 = a[ 0 ]
                k = c0.getElementsByTagName( "s:key" )
                for q in k :
                    if q . getAttribute( 'name' ) == 'dispatchState' :
                        status = q . firstChild . data
                    if q . getAttribute( 'name' ) == 'eventCount' :
                        count = q . firstChild . data
    except :
        pass
    return ( status, count )

def extract_raw_from_json( data : str ) :
    res = []
    j = json.loads( data )
    for r in j[ 'results' ] :
        res.append( r[ '_raw' ] )
    return res

class Searcher:

    def __init__(self, connectionFactory, bearer ):
        self.connFact = connectionFactory
        self.hdr = { 'Authorization' : "Bearer " + bearer }
        self.domain = "splunkapi.ncbi.nlm.nih.gov"

    # returns the search-id
    def create_search( self, search : str ) -> str :
        conn = self . connFact . HTTPSConnection( self.domain )
        data = urllib.parse.urlencode( { 'search' : search } )
        conn . request( 'POST', "/services/search/jobs/", data, self.hdr )
        req = conn . getresponse()
        return extract_search_id_from_xml( req.read() )

    def get_search_status( self, sid : str ) -> ( str, str ) :
        conn = self . connFact . HTTPSConnection( self.domain )
        conn . request( 'GET', "/services/search/jobs/" + sid, None, self.hdr )
        req = conn . getresponse()
        return extract_status_from_xml( req.read() )

    # return: ( the array of _raw lines, True if reached the end of search )
    def get_search_range( self, sid : str, offset : int, count : int ) -> ( [ str ], bool ) :
        conn = self . connFact . HTTPSConnection( self.domain )
        data = urllib.parse.urlencode( { "count" : count, "offset" : offset, "output_mode" : "json" } )
        path = "/services/search/jobs/" + sid + "/results?" + data
        conn . request( 'GET', path, None, self.hdr )
        req = conn . getresponse()
        res = extract_raw_from_json( req.read().decode() )
        return ( res, len( res ) < count )

    def print_search_results( self, sid : str, page_size : int ) -> str :
        done = False
        offset = 0
        while not done:
            res, done = self . get_search_range( sid, offset, page_size )
            for line in res :
                print( line )
            print( "offset="+str(offset) + " count="+str(count), file=sys.stderr )
            offset += len( res )

    def wait_for_done( self, sid : str, timeoutSec : int ) -> ( str, str ) :
        sleepSec = 1
        status = 'UNKNOWN'
        count = ""
        while timeoutSec > 0 :
            status, count = self . get_search_status( sid )
            if status == 'DONE' or status == 'FAILED' :
                return ( status, count )
            if timeoutSec % 10 == 0:
                print( "status="+status, file=sys.stderr )
            time.sleep( sleepSec )
            timeoutSec -= sleepSec
        return ( status, count )

def parseArgs( args : list ) -> argparse.Namespace :
    parser = argparse.ArgumentParser( description = "SPLUNK data retrieval tool" )
    parser.add_argument("--bearer", "-b", metavar="filename", type=str, dest="bearer", default="", help="name of the file with bearer token for Splunk access inside")
    parser.add_argument("--earliest", "-e", metavar="time", type=str, dest="earliest", default="-1m", help="start time")
    parser.add_argument("--latest", "-l", metavar="time", type=str, dest="latest", default="", help="end time")
    parser.add_argument("--timeout", "-t", metavar="timeout in seconds", type=int, dest="timeout", default="100", help="maximum wait time for the request, in seconds (default 100 seconds)")
    return parser.parse_args( args )

#TODO: allow overriding the search string or its components

if __name__ == "__main__":
    try:
        args = parseArgs( sys.argv[1:] )
        bearer = file2string( args.bearer )
        searcher = Searcher( http.client, bearer )

        search="search index=unix sourcetype=syslog host=cloudian-node* S3REQ bucketOwnerUserId=trace"
        search += " earliest=" + args.earliest
        if args.latest != "":
            search += " latest=" + args.latest

        sid = searcher . create_search( search )
        status, count = searcher . wait_for_done( sid, args.timeout )

        print( "status="+status, file=sys.stderr )
        print( "count="+count, file=sys.stderr )
        row_count = int( count )

        if status == 'DONE' :
            if row_count > 0 :
                searcher . print_search_results( sid, 100000 )
            else:
                print( "search returned no results", file=sys.stderr )
            sys.exit(0)
        else:
            if status == 'FAILED':
                print( "search failed", file=sys.stderr )
                sys.exit(3)
            else:
                print( "search timed out", file=sys.stderr )
                sys.exit(2)

    except Exception as ex:
        print( "Error: " + str( ex ), file=sys.stderr )
        sys.exit( 1 )

