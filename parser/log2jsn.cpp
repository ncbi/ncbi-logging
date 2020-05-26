#include "log_lines.hpp"
#include <sstream>
#include <string>

#include <ncbi/json.hpp>

using namespace std;
using namespace NCBI::Logging;
using namespace ncbi;

static string ToString( const t_str & in )
{
    return string("\"") + string ( in.p, in.n ) + "\"";
}

static JSONValueRef ToJsonString( const t_str & in )
{
    if ( in . n > 0 )
    {
        return JSON::makeString( String( in . p, in . n) );
    }
    else
    {
        return JSON::makeString( String () );
    }
}

static String FormatTime(t_timepoint t)
{
    ostringstream ret;
    ret << (int)t.day << "."
        << (int)t.month <<"."
        << (int)t.year <<":"
        << (int)t.hour <<":"
        << (int)t.minute <<":"
        << (int)t.second <<" "
        << (int)t.offset;
    return String(ret.str());
}

struct OpToJsonLogLines : public OP_LogLines
{
    virtual int unrecognized( const t_str & text )
    {
        JSONObjectRef j = JSON::makeObject();
        j -> addValue( "unrecognized", ToJsonString( text ) );

        mem_os << j->readableJSON().toSTLString() << endl;
        return 0;
    }

    virtual int acceptLine( const LogOPEvent & e )
    {
        mem_os << MakeJson(e, true)->readableJSON().toSTLString() << endl;
        return 0;
    }

    virtual int rejectLine( const LogOPEvent & e )
    {
        mem_os << MakeJson(e, false)->readableJSON().toSTLString() << endl;
        return 0;
    }

    JSONObjectRef MakeJson( const LogOPEvent & e, bool accepted )
    {
        JSONObjectRef j = JSON::makeObject();
        j -> addValue( "accepted", JSON::makeBoolean(accepted) );

        j -> addValue( "ip", ToJsonString( e.ip ) );
        j -> addValue( "time", JSON::makeString( FormatTime( e.time ) ) );

        {
            JSONObjectRef req = JSON::makeObject();
            req -> addValue("server",   ToJsonString( e.request.server ) );
            req -> addValue("method",   ToJsonString( e.request.method ) );
            req -> addValue("path",     ToJsonString( e.request.path ) );
            req -> addValue("params",   ToJsonString( e.request.params ) );
            req -> addValue("vers",     ToJsonString( e.request.vers ) );
            JSONValueRef rv( req.release() );
            j -> addValue( "request", rv );
        }

        j -> addValue( "res_code", JSON::makeInteger( e.res_code ) );
        j -> addValue( "res_len", JSON::makeInteger( e.res_len ) );
        j -> addValue( "referer", ToJsonString( e.referer ) );
        j -> addValue( "agent", ToJsonString( e.agent ) );

        j -> addValue( "user", ToJsonString( e.user ) );
        j -> addValue( "req_time", ToJsonString( e.req_time ) );
        j -> addValue( "forwarded", ToJsonString( e.forwarded ) );
        j -> addValue( "port", JSON::makeInteger( e.port ) );
        j -> addValue( "req_len", JSON::makeInteger( e.req_len ) );

        return j;
    }

    OpToJsonLogLines( ostream &os ) : mem_os( os ) {};

    ostream &mem_os;
};

extern "C"
{
    int main ( int argc, const char * argv [], const char * envp []  )
    {
        OpToJsonLogLines event_receiver( cout );
        OP_Parser p( event_receiver, cin );
        bool res = p . parse();

        return res ? 0 : 3;
    }
}
