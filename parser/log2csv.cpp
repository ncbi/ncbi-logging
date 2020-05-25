#include "log_lines.hpp"
#include <iostream>

using namespace std;
using namespace NCBI::Logging;

static void ToQuotedString( const t_str & in, ostream& out )
{
    //TODO: escape the contents as necessary
    if ( in.n == 0)
    {
        out << "";
    }
    else
    {
        out << "\"";
        out << string ( in.p, in.n );
        out << "\"";
    }
//    return string("\"") + string ( in.p, in.n ) + "\"";
}

static void FormatTime(t_timepoint t, ostream& out)
{
    out << "\""
        << (int)t.day << "."
        << (int)t.month <<"."
        << (int)t.year <<":"
        << (int)t.hour <<":"
        << (int)t.minute <<":"
        << (int)t.second <<" "
        << (int)t.offset
        << "\"";
}

struct CSV_OP_LogLines : public OP_LogLines
{
    virtual int unrecognized( const t_str & text )
    {
        mem_os << "unrecognized,";
        ToQuotedString ( text, mem_os );
        mem_os << "," << endl;
        return 0;
    }

    virtual int acceptLine( const LogOPEvent & event )
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

    virtual int rejectLine( const LogOPEvent & event )
    {
        mem_os << "rejected" << endl;
        return 0;
    }

    CSV_OP_LogLines( ostream &os ) : mem_os( os ) {};

    ostream &mem_os;
};

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

static int parse_op( void )
{
    CSV_OP_LogLines event_receiver( cout );
    OP_Parser p( event_receiver, cin );
    bool res = p . parse();

    return res ? 0 : 3;
}

static int parse_aws( void )
{
    CSV_AWS_LogLines event_receiver( cout );
    AWS_Parser p( event_receiver, cin );
    bool res = p . parse();

    return res ? 0 : 3;
}

int main ( int argc, const char * argv [], const char * envp []  )
{
    string fmt( "op" );
    if ( argc > 1 )
        fmt = string( argv[ 1 ] );

    if ( fmt == "aws" )
        return parse_aws();
    return parse_op();
}
