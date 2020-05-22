#include "log_lines.hpp"
#include <iostream>

using namespace std;
using namespace NCBI::Logging;

static string ToString( const t_str & in )
{
    return string( in.p == nullptr ? "" : in.p, in.n );
}

struct CSVLogLines : public LogLines
{
    virtual int unrecognized( const t_str & text )
    {
        mem_os << "unrecognized" << endl;
        return 0;
    }

    virtual int acceptLine( const LogOnPremEvent & event )
    {
        mem_os << "accepted" << endl;
        return 0;
    }

    virtual int rejectLine( const LogOnPremEvent & event )
    {
        mem_os << "rejected" << endl;
        return 0;
    }

    virtual int acceptLine( const LogAWSEvent & event )
    {
        return 0;
    }

    virtual int rejectLine( const LogAWSEvent & event )
    {
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
