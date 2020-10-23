#! /usr/bin/env python3

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

# returns the search-id
#TODO: refactor to have domain and path in one place
def create_search( bearer : str, search : str ) -> str :
    hdr = { 'Authorization' : "Bearer " + bearer }
    domain = "splunkapi.ncbi.nlm.nih.gov"
    path = "/services/search/jobs/"
    data = urllib.parse.urlencode( { 'search' : search } )
    conn = http.client.HTTPSConnection( domain )
    conn . request( 'POST', path, data, hdr )
    req = conn . getresponse()
    return extract_search_id_from_xml( req.read() )

def get_search_status( bearer : str, sid : str ) -> ( str, str ) :
    hdr = { 'Authorization' : "Bearer " + bearer }
    domain = "splunkapi.ncbi.nlm.nih.gov"
    path = "/services/search/jobs/" + sid
    conn = http.client.HTTPSConnection( domain )
    conn . request( 'GET', path, None, hdr )
    req = conn . getresponse()
    return extract_status_from_xml( req.read() )

# return: ( the array of _raw lines, True if reached the end of search )
def get_search_range( bearer : str, sid : str, offset : int, count : int ) -> ( [ str ], bool ) :
    hdr = { 'Authorization' : "Bearer " + bearer }
    domain = "splunkapi.ncbi.nlm.nih.gov"
    data = urllib.parse.urlencode( { "count" : count, "offset" : offset, "output_mode" : "json" } )
    path = "/services/search/jobs/" + sid + "/results?" + data
    conn = http.client.HTTPSConnection( domain )
    conn . request( 'GET', path, None, hdr )
    req = conn . getresponse()
    res = extract_raw_from_json( req.read().decode() )
    return ( res, len( res ) < count )

def print_search_results( bearer : str, sid : str, page_size : int ) -> str :
    done = False
    offset = 0
    while not done:
        res, done = get_search_range( bearer, sid, offset, page_size )
        for line in res :
           print( line )
        print( "offset="+str(offset) + " count="+str(count), file=sys.stderr )
        offset += len( res )

def wait_for_done( bearer : str, sid : str ) -> ( str, str ) :
    x = 200
    status = 'UNKNOWN'
    count = ""
    while x > 0 :
        status, count = get_search_status( bearer, sid )
        if status == 'DONE' :
            return ( status, count )
        if x % 10 == 0:
            print( "status="+status, file=sys.stderr )
        time.sleep( 0.5 )
        x -= 1
    return ( status, count )

#TODO: take the lookback time from the command line
#TODO: take page_size time from the command line
#TODO: take the wait time (wait_for_done) from the command line
#TODO: take the token file (or the token itself) from the command line
#TODO: allow overriding the search string or its components
#TODO: only print details in verbose mode

if __name__ == "__main__":
    try:
        search="search index=unix sourcetype=syslog host=cloudian-node* S3REQ bucketOwnerUserId=trace earliest=-50m"
        bearer = file2string( "bearer.txt" )

        sid = create_search( bearer, search )
        status, count = wait_for_done( bearer, sid )

        print( "status="+status, file=sys.stderr )
        print( "count="+count, file=sys.stderr )
        row_count = int( count )

        if status == 'DONE' and row_count > 0 :
            print_search_results( bearer, sid, 100000 )

    except Exception as ex:
        print( "Error reading bearer.txt: " + ex )
        sys.exit( 1 )

