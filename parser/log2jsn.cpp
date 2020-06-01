#include "log_lines.hpp"
#include <sstream>
#include <iostream>
#include <string>

#include <ncbi/json.hpp>

#include "helper.hpp"

using namespace std;
using namespace NCBI::Logging;
using namespace ncbi;

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

struct cmnLogLines
{
    JSONValueRef ToJsonString( const t_str & in )
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

    void print_json( JSONObjectRef j )
    {
        if ( mem_readable )
            mem_os << j->readableJSON().toSTLString() << endl;
        else
            mem_os << j->toJSON().toSTLString() << endl;
    }

    int cmn_unrecognized( const t_str & text )
    {
        JSONObjectRef j = JSON::makeObject();
        try
        {
            j -> addValue( "unrecognized", ToJsonString( text ) );
        }
        catch(const InvalidUTF8String& e)
        {
            j -> addValue( "unrecognized", JSON::makeString( "****Non-UTF8****" ) );
            std::cerr << "Non-UTF8 input:" << ToString(text) << '\n';
        }
        
        print_json( j );
        return 0;
    }

    void report( void )
    {
        unsigned long int total = num_accepted + num_rejected + num_unrecognized;
        std::cerr << "total : " << total << endl;
        std::cerr << "accepted : " << num_accepted << endl;
        std::cerr << "rejected : " << num_rejected << endl;
        std::cerr << "unrecognized : " << num_unrecognized << endl;
    }

    cmnLogLines( ostream &os, bool readable ) : mem_os( os ),
        mem_readable( readable ) {};

    ostream &mem_os;
    bool mem_readable;
    unsigned long int num_accepted = 0;
    unsigned long int num_rejected = 0;
    unsigned long int num_unrecognized = 0;
};

struct OpToJsonLogLines : public OP_LogLines, public cmnLogLines
{
    virtual int unrecognized( const t_str & text )
    {
        num_unrecognized ++;
        return cmn_unrecognized( text );
    }

    virtual int acceptLine( const LogOPEvent & e )
    {
        num_accepted ++;
        print_json( MakeJson(e, true) );
        return 0;
    }

    virtual int rejectLine( const LogOPEvent & e )
    {
        num_rejected ++;
        print_json( MakeJson(e, false) );
        return 0;
    }

    JSONObjectRef MakeJson( const LogOPEvent & e, bool accepted )
    {
        JSONObjectRef j = JSON::makeObject();
        j -> addValue( "accepted", JSON::makeBoolean(accepted) );
        j -> addValue( "source", JSON::makeString("NCBI") );
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

    OpToJsonLogLines( ostream &os, bool readable ) : cmnLogLines( os, readable ) {};
};

struct AWSToJsonLogLines : public AWS_LogLines , public cmnLogLines
{
    virtual int unrecognized( const t_str & text )
    {
        num_unrecognized ++;
        return cmn_unrecognized( text );
    }

    virtual int acceptLine( const LogAWSEvent & e )
    {
        num_accepted ++;
        print_json( MakeJson(e, true) );
        return 0;
    }

    virtual int rejectLine( const LogAWSEvent & e )
    {
        num_rejected ++;
        print_json( MakeJson(e, false) );
        return 0;
    }

    JSONObjectRef MakeJson( const LogAWSEvent & e, bool accepted )
    {
        JSONObjectRef j = JSON::makeObject();
        j -> addValue( "accepted", JSON::makeBoolean(accepted) );
        j -> addValue( "source", JSON::makeString("S3") );

        j -> addValue( "owner", ToJsonString( e.owner ) );
        j -> addValue( "bucket", ToJsonString( e.bucket ) );
        j -> addValue( "requester", ToJsonString( e.requester ) );
        j -> addValue( "request_id", ToJsonString( e.request_id ) );
        j -> addValue( "operation", ToJsonString( e.operation ) );
        j -> addValue( "key", ToJsonString( e.key ) );
        j -> addValue( "error", ToJsonString( e.error ) );

        j -> addValue( "obj_size", ToJsonString( e.obj_size ) );
        j -> addValue( "total_time", ToJsonString( e.total_time ) );
        j -> addValue( "turnaround_time", ToJsonString( e.turnaround_time ) );

        j -> addValue( "version_id", ToJsonString( e.version_id ) );
        j -> addValue( "host_id", ToJsonString( e.host_id ) );
        j -> addValue( "cipher_suite", ToJsonString( e.cipher_suite ) );
        j -> addValue( "auth_type", ToJsonString( e.auth_type ) );
        j -> addValue( "host_header", ToJsonString( e.host_header ) );
        j -> addValue( "tls_version", ToJsonString( e.tls_version ) );

        j -> addValue( "ip", ToJsonString( e.ip ) );
        j -> addValue( "time", JSON::makeString( FormatTime( e.time ) ) );

        j -> addValue( "request", ToJsonString( e.request ) );

        j -> addValue( "res_code", ToJsonString( e.res_code ) );
        j -> addValue( "res_len", ToJsonString( e.res_len ) );
        j -> addValue( "referer", ToJsonString( e.referer ) );
        j -> addValue( "agent", ToJsonString( e.agent ) );

        return j;
    }

    AWSToJsonLogLines( ostream &os, bool readable ) : cmnLogLines( os, readable ) {};
};

struct GCPToJsonLogLines : public GCP_LogLines , public cmnLogLines
{
    virtual int unrecognized( const t_str & text )
    {
        num_unrecognized ++;
        return cmn_unrecognized( text );
    }

    virtual int acceptLine( const LogGCPEvent & e )
    {
        num_accepted ++;
        print_json( MakeJson(e, true) );
        return 0;
    }

    virtual int rejectLine( const LogGCPEvent & e )
    {
        num_rejected ++;
        print_json( MakeJson(e, false) );
        return 0;
    }

    JSONObjectRef MakeJson( const LogGCPEvent & e, bool accepted )
    {
        JSONObjectRef j = JSON::makeObject();

        j -> addValue( "accepted", JSON::makeBoolean(accepted) );
        j -> addValue( "source", JSON::makeString("GS") );

        j -> addValue( "time", JSON::makeInteger( e.time ) );
        j -> addValue( "ip", ToJsonString( e.ip ) );
        j -> addValue( "ip_type", JSON::makeInteger( e.ip_type ) );
        j -> addValue( "ip_region", ToJsonString( e.ip_region ) );
        j -> addValue( "method", ToJsonString( e.method ) );
        j -> addValue( "uri", ToJsonString( e.uri ) );
        j -> addValue( "status", JSON::makeInteger( e.status ) );
        j -> addValue( "request_bytes", JSON::makeInteger( e.request_bytes ) );
        j -> addValue( "result_bytes", JSON::makeInteger( e.result_bytes ) );
        j -> addValue( "time_taken", JSON::makeInteger( e.time_taken ) );
        j -> addValue( "host", ToJsonString( e.host ) );
        j -> addValue( "referrer", ToJsonString( e.referrer ) );
        j -> addValue( "agent", ToJsonString( e.agent ) );
        j -> addValue( "request_id", ToJsonString( e.request_id ) );
        j -> addValue( "operation", ToJsonString( e.operation ) );
        j -> addValue( "bucket", ToJsonString( e.bucket ) );
        j -> addValue( "object", ToJsonString( e.object ) );

        return j;
    }

    GCPToJsonLogLines( ostream &os, bool readable ) : cmnLogLines( os, readable ) {};
};

static int handle_on_prem( bool readable, bool do_report ) 
{
    cerr << "converting on-premise format" << endl;
    OpToJsonLogLines event_receiver( cout, readable );
    OP_Parser p( event_receiver, cin );
    bool res = p . parse();
    if ( do_report )
        event_receiver.report();
    return res ? 0 : 3;
}

static int handle_aws( bool readable, bool do_report )
{
    cerr << "converting AWS format" << endl;
    AWSToJsonLogLines event_receiver( cout, readable );
    AWS_Parser p( event_receiver, cin );
    //p . setDebug( true );
    bool res = p . parse();
    if ( do_report )
        event_receiver.report();
    return res ? 0 : 3;
}

static int handle_gcp( bool readable, bool do_report ) 
{
    cerr << "converting GCP format" << endl;
    GCPToJsonLogLines event_receiver( cout, readable );
    GCP_Parser p( event_receiver, cin );
    //p . setDebug( true );
    bool res = p . parse();
    if ( do_report )
        event_receiver.report();
    return res ? 0 : 3;
}

extern "C"
{
    int main ( int argc, const char * argv [], const char * envp []  )
    {
        string format( "aws" );
        string readable( "" );
        if ( argc > 1 ) format = argv[ 1 ];
        if ( argc > 2 ) readable = argv[ 2 ];

        bool b_readable = ( readable == "readable" );

        if ( format == "op"  ) return handle_on_prem( b_readable, true );
        if ( format == "aws" ) return handle_aws( b_readable, true );
        if ( format == "gcp" ) return handle_gcp( b_readable, true );

        cerr << "unknown format: " << format << endl;
        return 3;
    }
}
