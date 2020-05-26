#include "log_lines.hpp"
#include <iostream>

using namespace std;
using namespace NCBI::Logging;

static string ToString( const t_str & in )
{
    if ( in.n == 0 )
    {
        return string();
    }   
    return string ( in.p, in.n );
}

string FormatMonth (uint8_t m)
{
    switch (m)
    {
    case 1: return "Jan";
    case 2: return "Feb";
    case 3: return "Mar";
    case 4: return "Apr";
    case 5: return "May";
    case 6: return "Jun";
    case 7: return "Jul";
    case 8: return "Aug";
    case 9: return "Sep";
    case 10: return "Oct";
    case 11: return "Nov";
    case 12: return "Dec";
    default: return "???";
    }
}

static void FormatTime(t_timepoint t, ostream& out)
{
    out << "[";
    if ( t.day < 10 ) out << "0";
    out << (int)t.day << "/";

    out << FormatMonth( t.month )<<"/";

    out << (int)t.year <<":";
    if ( t.hour < 10 ) out << "0";
    out << (int)t.hour <<":";
    if ( t.minute < 10 ) out << "0";
    out << (int)t.minute <<":";
    if ( t.second < 10 ) out << "0";
    out << (int)t.second <<" ";
    if ( t.offset < 0 ) out << "-";
    if ( abs(t.offset) < 1000 ) out << "0";
    out << abs((int)t.offset);
    out << "]";
}

static void FormatRequest(t_request r, ostream& out)
{
    if ( r.server.n > 0 )
    {
        out << "\"" << ToString( r.server ) << "\" ";
    }
    out << "\"" << ToString( r.method ) << " "
        << ToString( r.path );
    if ( r . params . n > 0 )
    {
        out << "?" << ToString( r.params );
    }
    out << " " << ToString( r.vers );
    out << "\" ";
}

struct SRC_OP_LogLines : public OP_LogLines
{
    virtual int unrecognized( const t_str & text )
    {
        mem_os << ToString(text) <<endl;
        return 0;
    }

    virtual int acceptLine( const LogOPEvent & event )
    {
// 158.111.236.250 - - [01/Jan/2020:02:50:24 -0500] "sra-download.ncbi.nlm.nih.gov" "GET /traces/sra34/SRR/003923/SRR4017927 HTTP/1.1" 206 32768 0.000 "-" "linux64 sra-toolkit fastq-dump.2.9.1" "-" port=443 rl=293

        mem_os << ToString ( event . ip ) << " - ";
        if ( event.user.n == 0 )
        {
            mem_os << "- ";
        }
        else
        {
            mem_os << ToString ( event . user ) << " - ";
        }
        
        FormatTime ( event.time, mem_os ); mem_os << " ";
        FormatRequest ( event . request, mem_os );
        mem_os << event.res_code << " ";
        mem_os << event.res_len << " ";
        mem_os << ToString ( event . req_time ) << " ";
        mem_os << "\"" << ToString ( event . referer ) << "\" ";
        mem_os << "\"" << ToString ( event . agent ) << "\" ";
        mem_os << "\"" << ToString ( event . forwarded ) << "\" ";
        mem_os << "port=" << event . port << " ";
        mem_os << "rl=" << event . req_len ;
        mem_os << endl;
        return 0;
    }

    virtual int rejectLine( const LogOPEvent & event )
    {
        mem_os << "rejected" << endl;
        return 0;
    }

    SRC_OP_LogLines( ostream &os, ostream &err ) 
    : mem_os( os ) , mem_err ( err )
    {}

    ostream &mem_os;
    ostream &mem_err;
};
#if 0
struct CSV_AWS_LogLines : public AWS_LogLines
{
    virtual int unrecognized( const t_str & text )
    {
        mem_os << "unrecognized,";
        ToQuotedString ( text, mem_os );
        mem_os << "," << endl;
        return 0;
    }

    virtual int acceptLine( const LogAWSEvent & event )
    {
        ToQuotedString ( event . ip, mem_os ); mem_os << ",";
        FormatTime ( event.time, mem_os ); mem_os << ",";
        ToQuotedString ( event . request.server, mem_os ); mem_os << ",";
        ToQuotedString ( event . request.method, mem_os ); mem_os << ",";
        ToQuotedString ( event . request.path, mem_os ); mem_os << ",";
        ToQuotedString ( event . request.params, mem_os ); mem_os << ",";
        ToQuotedString ( event . request.vers, mem_os ); mem_os << ",";
        mem_os << endl;
        return 0;
    }

    virtual int rejectLine( const LogAWSEvent & event )
    {
        mem_os << "rejected" << endl;
        return 0;
    }

    CSV_AWS_LogLines( ostream &os ) : mem_os( os ) {};

    ostream &mem_os;
};
#endif

static int parse_op( void )
{
    SRC_OP_LogLines event_receiver( cout, cerr );
    OP_Parser p( event_receiver, cin );
    //p.setDebug(true);
    bool res = p . parse();

    return res ? 0 : 3;
}
#if 0
static int parse_aws( void )
{
    CSV_AWS_LogLines event_receiver( cout );
    AWS_Parser p( event_receiver, cin );
    bool res = p . parse();

    return res ? 0 : 3;
}
#endif
int main ( int argc, const char * argv [], const char * envp []  )
{
    string fmt( "op" );
    if ( argc > 1 )
        fmt = string( argv[ 1 ] );

    // if ( fmt == "aws" )
    //     return parse_aws();
    return parse_op();
}
