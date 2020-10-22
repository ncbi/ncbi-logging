%define api.pure full
%lex-param { void * scanner }
%parse-param { void * scanner }{ NCBI::Logging::CLReceiver * lib }

%define parse.trace
%define parse.error verbose

%name-prefix "cl_"

%{
#define YYDEBUG 1

#include <stdint.h>
#include "cl_parser.hpp"
#include "cl_scanner.hpp"

using namespace std;
using namespace NCBI::Logging;

void cl_error( yyscan_t locp, NCBI::Logging::CLReceiver * lib, const char* msg );

const t_str EmptyTSTR = { "", 0 };

%}

%code requires
{
#include "types.h"
#include "CL_Interface.hpp"

using namespace NCBI::Logging;
}

%token<s> UNRECOGNIZED DATETIME1 DATETIME2 DATETIME3 STR REQID SPACE PIPE

%type<s> str_opt

%union
{
    t_str s;
}

%start line

%%

line
    : cl_line { YYACCEPT; }
    | error  { YYABORT; }
    ;

cl_line
    : syslog_prefix SPACE
      timestamp PIPE
      ip PIPE
      owner PIPE
      method PIPE
      bucket PIPE
      unknown1 PIPE
      requestHdrSize PIPE
      requestBodySize PIPE
      responseHdrSize PIPE
      responseBodySize PIPE
      totalSize PIPE
      unknown2 PIPE
      path PIPE
      httpStatus PIPE
      reqId PIPE
      unknown3 PIPE
      eTag PIPE
      errorCode
    ;

syslog_prefix
    : DATETIME1 SPACE
      STR SPACE
      STR SPACE
      DATETIME2 SPACE
      STR SPACE
      STR SPACE
      STR SPACE
      STR SPACE
      REQID
      {
          t_str prefix = $1;
          MERGE_TSTR( prefix, $2 );
          MERGE_TSTR( prefix, $3 );
          MERGE_TSTR( prefix, $4 );
          MERGE_TSTR( prefix, $5 );
          MERGE_TSTR( prefix, $6 );
          MERGE_TSTR( prefix, $7 );
          MERGE_TSTR( prefix, $8 );
          MERGE_TSTR( prefix, $9 );
          MERGE_TSTR( prefix, $10 );
          MERGE_TSTR( prefix, $11 );
          MERGE_TSTR( prefix, $12 );
          MERGE_TSTR( prefix, $13 );
          MERGE_TSTR( prefix, $14 );
          MERGE_TSTR( prefix, $15 );
          MERGE_TSTR( prefix, $16 );
          MERGE_TSTR( prefix, $17 );
          lib -> set( CLReceiver::syslog_prefix, prefix );
      }
    ;

timestamp
    : DATETIME3 { lib -> set( CLReceiver::timestamp, $1 ); }
    ;

str_opt
    : STR       { $$ = $1; }
    | %empty    { $$ = EmptyTSTR; }
    ;

ip:                 str_opt { lib -> set( ReceiverInterface::ip, $1 ); } ;
owner:              str_opt { lib -> set( CLReceiver::owner, $1 ); } ;
method:             str_opt { lib -> set( ReceiverInterface::method, $1 ); } ;
bucket:             str_opt { lib -> set( CLReceiver::bucket, $1 ); } ;
unknown1:           str_opt { lib -> set( CLReceiver::unknown1, $1 ); } ;
requestHdrSize:     str_opt { lib -> set( CLReceiver::requestHdrSize, $1 ); } ;
requestBodySize:    str_opt { lib -> set( CLReceiver::requestBodySize, $1 ); } ;
responseHdrSize:    str_opt { lib -> set( CLReceiver::responseHdrSize, $1 ); } ;
responseBodySize:   str_opt { lib -> set( CLReceiver::responseBodySize, $1 ); } ;
totalSize:          str_opt { lib -> set( CLReceiver::totalSize, $1 ); } ;
unknown2:           str_opt { lib -> set( CLReceiver::unknown2, $1 ); } ;
path:               str_opt
                        {
                            lib -> set( ReceiverInterface::path, $1 );
                            lib -> obj_for_postprocess = string( $1.p, $1.n );
                        } ;
httpStatus:         str_opt { lib -> set( CLReceiver::httpStatus, $1 ); } ;
reqId:              str_opt { lib -> set( CLReceiver::reqId, $1 ); } ;
unknown3:           str_opt { lib -> set( CLReceiver::unknown3, $1 ); } ;
eTag:               str_opt { lib -> set( CLReceiver::eTag, $1 ); } ;
errorCode:          str_opt { lib -> set( CLReceiver::errorCode, $1 ); } ;

/*
datetime    Oct 20 15:27:38
src         cloudian-node-120.be-md.ncbi.nlm.nih.gov
linecount   1
timestamp2  2020-10-20T15:27:37.592-04:00
dest            cloudian-node-120.be-md.ncbi.nlm.nih.gov
syslogEvent     S3REQ
durationMsec    4738
                -
REQID          [mdc@18060 REQID="45200f16-ad2e-1945-87cb-d8c49756ebf4"]

timestamp    2020-10-20 15:27:36,999
    |
ip    10.10.26.17
    |
owner    trace
    |
operation     getObject
    |
bucketName    sra-pub-run-5
    |
unknown1(timestartpos?)
    |
requestHdrSize    252
    |
requestBodySize    0
    |
responseHdrSize    256
    |
responseBodySize    20971520
    |
totalSize    20972028
    |
unknown2    593542
    |
objectName    ERR527068%2FERR527068.1
    |
httpStatus    206
    |
S3reqId    45200f16-ad2e-1945-87cb-d8c49756ebf4
    |
unknown3    c89056a1918b4ea6534c912deee13f11-154
    |
eTag    0
    |
errorCode
*/

%%

void cl_error( yyscan_t locp, NCBI::Logging::CLReceiver * lib, const char * msg )
{
    // intentionally left empty, we communicate errors rejected lines
}
