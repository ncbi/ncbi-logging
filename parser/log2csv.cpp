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

struct CSVLogLines : public LogLines
{
    virtual int unrecognized( const t_str & text )
    {
        mem_os << "unrecognized,";
        ToQuotedString ( text, mem_os );
        mem_os << "," << endl;
        return 0;
    }

    virtual int acceptLine( const CommonLogEvent & event )
    {
        switch( event . m_format )
        {
            case e_onprem :
            {
                const LogOnPremEvent& e = reinterpret_cast<const LogOnPremEvent&>(event);
                ToQuotedString ( e . ip, mem_os ); mem_os << ",";
                FormatTime ( e.time, mem_os ); mem_os << ",";
                ToQuotedString ( e . request.server, mem_os ); mem_os << ",";
                ToQuotedString ( e . request.method, mem_os ); mem_os << ",";
                ToQuotedString ( e . request.path, mem_os ); mem_os << ",";
                ToQuotedString ( e . request.params, mem_os ); mem_os << ",";
                ToQuotedString ( e . request.vers, mem_os ); mem_os << ",";
                mem_os << endl;
            }
            break;

            default : throw logic_error( "bad event format accepted" ); break;
        }
        return 0;
    }

    virtual int rejectLine( const CommonLogEvent & event )
    {
        mem_os << "rejected" << endl;
        return 0;
    }

    CSVLogLines( ostream &os ) : mem_os( os ) {};

    ostream &mem_os;
};

extern "C"
{
    int main ( int argc, const char * argv [], const char * envp []  )
    {
        CSVLogLines event_receiver( cout );
        LogParser p( event_receiver, cin );
        bool res = p . parse();

        return res ? 0 : 3;
    }
}
