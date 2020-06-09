#include "log_lines.hpp"
#include <sstream>
#include <iostream>
#include <string>

#include <ncbi/json.hpp>
#include <ncbi/secure/except.hpp>
#include <ncbi/secure/string.hpp>
#include <cmdline.hpp>

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

static String FormatTime( const t_timepoint & t )
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

    void cmn_unrecognized( const t_str & text )
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
    }

    void report( void )
    {
        unsigned long int total = num_accepted + num_rejected + num_unrecognized + num_headers;
        std::cerr << "total : " << total << endl;
        std::cerr << "accepted : " << num_accepted << endl;
        std::cerr << "rejected : " << num_rejected << endl;
        if ( num_headers > 0 )
            std::cerr << "headers : " << num_headers << endl;
        std::cerr << "unrecognized : " << num_unrecognized << endl;
    }

    cmnLogLines( ostream &os, bool readable, bool print_line_nr ) : mem_os( os ),
        mem_readable( readable ), mem_print_line_nr( print_line_nr ) {};

    ostream &mem_os;
    bool mem_readable;
    bool mem_print_line_nr;
    unsigned long int num_accepted = 0;
    unsigned long int num_rejected = 0;
    unsigned long int num_headers = 0;
    unsigned long int num_unrecognized = 0;
    unsigned long int line_nr = 0;
};

struct OpToJsonLogLines : public OP_LogLines, public cmnLogLines
{
    virtual void unrecognized( const t_str & text )
    {
        num_unrecognized ++;
        cmn_unrecognized( text );
        line_nr ++;
    }

    virtual void acceptLine( const LogOPEvent & e )
    {
        num_accepted ++;
        print_json( MakeJson(e, true) );
        line_nr ++;
    }

    virtual void rejectLine( const LogOPEvent & e )
    {
        num_rejected ++;
        print_json( MakeJson(e, false) );
        line_nr ++;
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

        if ( mem_print_line_nr )
            j -> addValue( "line_nr", JSON::makeInteger( line_nr ) );
        return j;
    }

    OpToJsonLogLines( ostream &os, bool readable, bool print_line_nr ) : cmnLogLines( os, readable, print_line_nr ) {};
};

struct AWSToJsonLogLines : public AWS_LogLines , public cmnLogLines
{
    virtual void unrecognized( const t_str & text )
    {
        num_unrecognized ++;
        cmn_unrecognized( text );
        line_nr ++;
    }

    virtual void acceptLine( const LogAWSEvent & e )
    {
        num_accepted ++;
        print_json( MakeJson(e, true) );
        line_nr ++;
    }

    virtual void rejectLine( const LogAWSEvent & e )
    {
        num_rejected ++;
        print_json( MakeJson(e, false) );
        line_nr ++;
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

        if ( mem_print_line_nr )
            j -> addValue( "line_nr", JSON::makeInteger( line_nr ) );

        return j;
    }

    AWSToJsonLogLines( ostream &os, bool readable, bool print_line_nr ) : cmnLogLines( os, readable, print_line_nr ) {};
};

struct GCPToJsonLogLines : public GCP_LogLines , public cmnLogLines
{
    virtual void unrecognized( const t_str & text )
    {
        num_unrecognized ++;
        cmn_unrecognized( text );
        line_nr ++;
    }

    virtual void acceptLine( const LogGCPEvent & e )
    {
        num_accepted ++;
        print_json( MakeJson(e, true) );
        line_nr ++;
    }

    virtual void rejectLine( const LogGCPEvent & e )
    {
        num_rejected ++;
        print_json( MakeJson(e, false) );
        line_nr ++;
    }

    virtual void headerLine( const LogGCPHeader & hdr )
    {
        JSONObjectRef obj = JSON::makeObject();
        obj -> addValue( "header", JSON::makeBoolean(true) );

        JSONArrayRef j = JSON::makeArray();
        for (auto i=hdr.m_fieldnames.begin(); i != hdr.m_fieldnames.end(); ++i)
        {
            j -> appendValue( JSON::makeString( String( *i ) ) );
        }

        JSONValueRef v = j->clone(); //TODO: is clone() really necessary?
        obj -> addValue( String("fields"), v );

        num_headers ++;

        print_json( obj );
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

        if ( mem_print_line_nr )
            j -> addValue( "line_nr", JSON::makeInteger( line_nr ) );

        return j;
    }

    GCPToJsonLogLines( ostream &os, bool readable, bool print_line_nr ) : cmnLogLines( os, readable, print_line_nr ) {};
};

static void handle_on_prem( bool readable, bool do_report, bool print_line_nr, bool do_debug = false ) 
{
    cerr << "converting on-premise format" << endl;
    OpToJsonLogLines event_receiver( cout, readable, print_line_nr );
    OP_Parser p( event_receiver, cin );
    p . setDebug( do_debug );
    p . parse();
    if ( do_report )
        event_receiver.report();
}

static void handle_aws( bool readable, bool do_report, bool print_line_nr, bool do_debug = false )
{
    cerr << "converting AWS format" << endl;
    AWSToJsonLogLines event_receiver( cout, readable, print_line_nr );
    AWS_Parser p( event_receiver, cin );
    p . setDebug( do_debug );
    p . parse();
    if ( do_report )
        event_receiver.report();
}

static void handle_gcp( bool readable, bool do_report, bool print_line_nr, bool do_debug = false ) 
{
    cerr << "converting GCP format" << endl;
    GCPToJsonLogLines event_receiver( cout, readable, print_line_nr );
    GCP_Parser p( event_receiver, cin );
    p . setDebug( do_debug );
    p . parse();
    if ( do_report )
        event_receiver.report();
}

enum logformat{ op, aws, gcp, unknown };

static logformat string2logformat( const ncbi::String & fmt )
{
    if ( fmt.equal( "op" ) )       { return op; }
    else if ( fmt.equal( "aws" ) ) { return aws; }
    else if ( fmt.equal( "gcp" ) ) { return gcp; }
    return unknown;
}

static int perform_parsing( logformat fmt, bool readable, bool do_report,
        bool print_line_nr, bool do_debug = false )
{
    switch( fmt )
    {
        case op  : handle_on_prem( readable, do_report, print_line_nr, do_debug ); break;
        case aws : handle_aws( readable, do_report, print_line_nr, do_debug ); break;
        case gcp : handle_gcp( readable, do_report, print_line_nr, do_debug ); break;
        default  : {
                        cerr << "Unknown format " << endl;
                        return 3;
                   }
    }
    return 0;
}

int main ( int argc, char * argv [], const char * envp []  )
{
    try
    {
        ncbi::String format( "op" );
        bool help = false;
        bool readable = false;
        bool no_report = false;
        bool print_line_nr = false;
        bool debug = false;
        bool vers = false;

        ncbi::Cmdline args( argc, argv );
        args . addOption( readable, "r", "readable", "pretty print json output" );
        args . addOption( debug, "d", "debug", "parse with debug-output" );
        args . addOption( no_report, "n", "no-report", "supress report" );
        args . addOption( print_line_nr, "p", "print-line-nr", "print line numbers" );
        args . addOption( vers, "V", "version", "show version" );
        args . addOption( help, "h", "help", "show help" );

        args . startOptionalParams();
        args . addParam( format, "format", "set the input format [ op, aws, gcp ] - dflt:op" );

        args . parse();

        if ( help )
            args . help();

        if ( vers )
            cout << "version: 1.0" << endl;
        
        if ( !help && !vers )
            return perform_parsing( string2logformat( format ), readable, !no_report, print_line_nr, debug );
        else
            return 0;
    }
    catch( const exception & e)
    {
        cerr << "Exception caught: " << e.what() << endl;
        return 1;
    }
    catch(...)
    {
        cerr << "Unknown exception caught" << endl;
        return 2;
    }
    return 0;
}
