#!/usr/bin/python3

import sys, json

def validateJSON( jsonData ):
    try:
        json.loads( jsonData )
    except ValueError as err:
        return False
    return True

def validate_f( f ) :
    errors = 0
    for line in f:
        if validateJSON( line.strip() ) :
            sys.stdout.write( line )
        else :
            sys.stderr.write( line )
            errors += 1
    return errors

if __name__ == "__main__":
    errors = 0
    if len( sys.argv ) > 0 :
        with open( sys.argv[ 1 ] ) as f:
            errors = validate_f( f )
    else :
        errors = validate_f( sys.stdin )
    if errors > 0 :
        sys.exit( 3 )
