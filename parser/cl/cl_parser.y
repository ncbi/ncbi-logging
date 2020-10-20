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

#define SET_VALUE( selector, source )   ( lib -> set( (selector), (source) ) )

%}

%code requires
{
#include "types.h"
#include "CL_Interface.hpp"

using namespace NCBI::Logging;
}

%token<s> UNRECOGNIZED

%union
{
    t_str s;
}

%start line

%%

line
    : error  { YYABORT; }
    ;

/*
datetime    Oct 20 15:27:38
src         cloudian-node-120.be-md.ncbi.nlm.nih.gov
linecount   1
timestamp2  2020-10-20T15:27:37.592-04:00

dest            cloudian-node-120.be-md.ncbi.nlm.nih.gov

syslogEvent     S3REQ

durationMsec    4738

REQID          - [mdc@18060 REQID="45200f16-ad2e-1945-87cb-d8c49756ebf4"]

timestamp1    2020-10-20 15:27:36,999
    |
ipAddress    10.10.26.17
    |
bucketOwnerUserId    trace
    |
operation     getObject
    |
bucketName    sra-pub-run-5
    |
timestartpos?
    |
requestHeaderSize    252
    |
requestBodySize    0
    |
responseHeaderSize    256
    |
responseBodySize    20971520
    |
totalRequestResponseSize    20972028
    |
unknown1    593542
    |
objectName    ERR527068%2FERR527068.1
    |
httpStatus    206
    |
S3reqId    45200f16-ad2e-1945-87cb-d8c49756ebf4
    |
unknown1    c89056a1918b4ea6534c912deee13f11-154
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
