#include "log_lines.hpp"

#include <iostream>
#include <sstream>
#include <iomanip>

#include "helper.hpp"

using namespace std;
using namespace NCBI::Logging;

static string FormatRequest(const t_request & r)
{
    ostringstream out;    
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

    return out.str();
}

struct SRC_OP_LogLines : public OP_LogLines
{
    string ToString( const t_str & str )
    {
        t_str s ( str );
        s . escaped = false;
        return NCBI::Logging::ToString( s );
    }

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
        
        mem_os << event.time.ToString() << " ";
        mem_os << FormatRequest ( event . request );
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

    SRC_OP_LogLines( ostream &os ) 
    : mem_os( os )
    {}

    ostream &mem_os;
};

struct SRC_AWS_LogLines : public AWS_LogLines
{
    virtual int unrecognized( const t_str & text )
    {
        mem_os << ToString(text) <<endl;
        return 0;
    }

    virtual int acceptLine( const LogAWSEvent & event )
    {   //TODO: complete
        mem_os << ToString ( event . owner ) << " ";
        mem_os << ToString ( event . bucket ) << " ";
        mem_os << event.time.ToString() << " ";
        mem_os << ToString ( event . ip ) << " ";
        mem_os << ToString ( event . requester ) << " ";
        mem_os << ToString ( event . request_id ) << " ";
        mem_os << ToString ( event . operation ) << " ";
        mem_os << ToString ( event . key ) << " ";
        mem_os << ToString ( event . request ) << " ";
        mem_os << ToString( event . res_code ) << " ";
        mem_os << ToString ( event . error ) << " ";
        mem_os << ToString( event.res_len ) << " ";
        mem_os << ToString( event.obj_size ) << " ";
        mem_os << ToString( event.total_time ) << " ";
        mem_os << ToString( event.turnaround_time ) << " ";
        mem_os << "\"" << ToString ( event . referer ) << "\" ";
        mem_os << "\"" << ToString ( event . agent ) << "\" ";
        mem_os << ToString ( event . version_id ) << " ";
        mem_os << ToString ( event . host_id ) << " ";
        mem_os << endl;
        return 0;
    }

    virtual int rejectLine( const LogAWSEvent & event )
    {
        mem_os << "rejected" << endl;
        return 0;
    }

    SRC_AWS_LogLines( ostream &os ) : mem_os( os ) {};

    ostream &mem_os;
};

static int parse_op( void )
{
    SRC_OP_LogLines event_receiver( cout );
    OP_Parser p( event_receiver, cin );
    //p.setDebug(true);
    bool res = p . parse();

    return res ? 0 : 3;
}

static int parse_aws( void )
{
    SRC_AWS_LogLines event_receiver( cout );
    AWS_Parser p( event_receiver, cin );
    bool res = p . parse();

    return res ? 0 : 3;
}

//TODO: add GCP

int main ( int argc, const char * argv [], const char * envp []  )
{
    string fmt( "op" );
    if ( argc > 1 )
        fmt = string( argv[ 1 ] );

    if ( fmt == "aws" )
       return parse_aws();
    return parse_op();
}
