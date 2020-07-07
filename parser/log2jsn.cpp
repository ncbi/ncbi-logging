#include "log_lines.hpp"
#include <sstream>
#include <iostream>
#include <iomanip>
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
    std::string s( ToString( t ) );
    return String( s );
}

ostream& operator<< (ostream& os, const t_str & s)
{
    os << "\"";
    for ( auto i = 0; i < s.n; ++i )
    {
        switch ( s.p[i] )
        {
        case '\\':
        case '\"':
            os << '\\';
            os << s.p[i];
            break;
        default:
            if ( s.p[i] < 0x20 )
            {
               ostringstream temp;
               temp << "\\u";
               temp << hex << setfill('0') << setw(4);
               temp << (int)s.p[i];
               os << temp.str();
            }
            else
            {
                os << s.p[i];
            }
            break;
        }
    }
    os << "\"";
    return os;
}

ostream& operator<< (ostream& os, const t_timepoint & t)
{
    os << "\"" << ToString( t ) << "\"";
    return os;
}

struct Options
{
    bool readable = false;
    bool report = true;
    bool print_line_nr = false;
    bool parser_debug = false;
    bool jsonlib = false;
    bool path_only = false;
    unsigned long int selected_line = 0;
};

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

    void MakeCmnJson( JSONObjectRef & j, const CommonLogEvent & e, bool accepted )
    {
        if ( accepted )
        {
            j -> addValue( "accepted", JSON::makeBoolean(true) );
        }
        else
        {
            j -> addValue( "accepted", JSON::makeBoolean(false) );
            j -> addValue( "unparsed", ToJsonString( e.unparsed ) );
        }
        j -> addValue( "ip", ToJsonString( e.ip ) );
        j -> addValue( "referer", ToJsonString( e.referer ) );
        j -> addValue( "agent", ToJsonString( e.agent ) );

        if ( mem_options . print_line_nr )
            j -> addValue( "line_nr", JSON::makeInteger( line_nr ) );

        j -> addValue( "method",    ToJsonString( e.request.method ) );
        j -> addValue( "path",      ToJsonString( e.request.path ) );
        j -> addValue( "vers",      ToJsonString( e.request.vers ) );
        j -> addValue( "accession", ToJsonString( e.request.accession ) );
        j -> addValue( "filename", ToJsonString( e.request.filename ) );
        j -> addValue( "extension", ToJsonString( e.request.extension ) );
        // e.request.server is only output for OP
    }

    void print_json( JSONObjectRef j )
    {
        if ( mem_options . readable )
            mem_os << j->readableJSON().toSTLString() << endl;
        else
            mem_os << j->toJSON().toSTLString() << endl;
    }

    void cmn_unrecognized( const t_str & text )
    {
        num_unrecognized ++;
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
        line_nr ++;
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

    cmnLogLines( ostream &os, const Options & options ) : mem_os( os ),
        mem_options( options ) {};

    ostream &mem_os;
    Options mem_options;
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
        if ( mem_options . path_only ) return;
        cmn_unrecognized( text );
    }

    virtual void acceptLine( const LogOPEvent & e )
    {
        num_accepted ++;
        line_nr ++;

        if ( mem_options . jsonlib )
        {
            print_json( MakeJson(e, true) );
        }
        else
        {
            print_direct(e, true);
        }
    }

    virtual void rejectLine( const LogOPEvent & e )
    {
        num_rejected ++;
        line_nr ++;

        if ( mem_options . jsonlib )
        {
            print_json( MakeJson( e, false) );
        }
        else
        {
            print_direct(e, false);
        }
    }

    void print_direct ( const LogOPEvent & e, bool accepted )
    {
        if ( mem_options . path_only )
        {
            if ( e. request . path . n > 0 )
            {
                mem_os << e.request.path << endl;
            }
            return;
        }
        mem_os << "{\"accepted\":" << (accepted ? "true":"false");
        mem_os << ",\"accession\":" << e.request.accession;
        mem_os << ",\"agent\":" << e.agent;
        mem_os << ",\"extension\":" << e.request.extension;
        mem_os << ",\"filename\":" << e.request.filename;
        mem_os << ",\"forwarded\":" << e.forwarded;
        mem_os << ",\"ip\":" << e.ip;
        if ( mem_options . print_line_nr )
        {
            mem_os << ",\"line_nr\":" << line_nr;
        }
        mem_os << ",\"method\":" << e.request.method;
        mem_os << ",\"path\":" << e.request.path;
        mem_os << ",\"port\":" << e.port;
        mem_os << ",\"referer\":" << e.referer;
        mem_os << ",\"req_len\":" << e.req_len;
        mem_os << ",\"req_time\":" << e.req_time;
        mem_os << ",\"res_code\":" << e.res_code;
        mem_os << ",\"res_len\":" << e.res_len;
        mem_os << ",\"server\":" << e.request.server;
        mem_os << ",\"source\":\"NCBI\"";
        mem_os << ",\"time\":" << e.time;
        if ( !accepted )
        {
            mem_os << ",\"unparsed\":" << e.unparsed;
        }
        mem_os << ",\"user\":" << e.user;
        mem_os << ",\"vers\":" << e.request.vers;
        mem_os << "}" <<endl;
    }

    JSONObjectRef MakeJson( const LogOPEvent & e, bool accepted )
    {
        JSONObjectRef j = JSON::makeObject();

        MakeCmnJson( j, e, accepted );
        j -> addValue( "source", JSON::makeString("NCBI") );
        j -> addValue( "time", JSON::makeString( FormatTime( e.time ) ) );
        j -> addValue( "server",   ToJsonString( e.request.server ) );
        j -> addValue( "res_code", JSON::makeInteger( e.res_code ) );
        j -> addValue( "res_len", JSON::makeInteger( e.res_len ) );
        j -> addValue( "user", ToJsonString( e.user ) );
        j -> addValue( "req_time", ToJsonString( e.req_time ) );
        j -> addValue( "forwarded", ToJsonString( e.forwarded ) );
        j -> addValue( "port", JSON::makeInteger( e.port ) );
        j -> addValue( "req_len", JSON::makeInteger( e.req_len ) );

        return j;
    }

    OpToJsonLogLines( ostream &os, const Options& options ) : cmnLogLines( os, options ) {};
};

struct AWSToJsonLogLines : public AWS_LogLines , public cmnLogLines
{
    virtual void unrecognized( const t_str & text )
    {
        if ( mem_options . path_only ) return;
        cmn_unrecognized( text );
    }

    virtual void acceptLine( const LogAWSEvent & e )
    {
        num_accepted ++;
        line_nr ++;

        if ( mem_options . jsonlib )
        {
            print_json( MakeJson( e, true ) );
        }
        else
        {
            print_direct(e, true );
        }
    }

    virtual void rejectLine( const LogAWSEvent & e )
    {
        num_rejected ++;
        line_nr ++;

        if ( mem_options . jsonlib )
        {
            print_json( MakeJson( e, false) );
        }
        else
        {
            print_direct(e, false);
        }
    }

    void print_direct ( const LogAWSEvent & e, bool accepted )
    {
        if ( mem_options . path_only )
        {
            if ( e. request . path . n > 0 )
            {
                mem_os << e.request.path << endl;
            }
            return;
        }
        mem_os << "{\"accepted\":" << (accepted ? "true":"false");
        mem_os << ",\"accession\":" << e.request.accession;
        mem_os << ",\"agent\":" << e.agent;
        mem_os << ",\"auth_type\":" << e.auth_type;
        mem_os << ",\"bucket\":" << e.bucket;
        mem_os << ",\"cipher_suite\":" << e.cipher_suite;
        mem_os << ",\"error\":" << e.error;
        mem_os << ",\"extension\":" << e.request.extension;
        mem_os << ",\"filename\":" << e.request.filename;
        mem_os << ",\"host_header\":" << e.host_header;
        mem_os << ",\"host_id\":" << e.host_id;
        mem_os << ",\"ip\":" << e.ip;
        mem_os << ",\"key\":" << e.key;
        if ( mem_options . print_line_nr )
        {
            mem_os << ",\"line_nr\":" << line_nr;
        }
        mem_os << ",\"method\":" << e.request.method;
        mem_os << ",\"obj_size\":" << e.obj_size;
        mem_os << ",\"operation\":" << e.operation;
        mem_os << ",\"owner\":" << e.owner;
        mem_os << ",\"path\":" << e.request.path;
        mem_os << ",\"referer\":" << e.referer;
        mem_os << ",\"request_id\":" << e.request_id;
        mem_os << ",\"requester\":" << e.requester;
        mem_os << ",\"res_code\":" << e.res_code;
        mem_os << ",\"res_len\":" << e.res_len;
        mem_os << ",\"source\":\"S3\"";
        mem_os << ",\"time\":" << e.time;
        mem_os << ",\"tls_version\":" << e.tls_version;
        mem_os << ",\"total_time\":" << e.total_time;
        mem_os << ",\"turnaround_time\":" << e.turnaround_time;
        if ( !accepted )
        {
            mem_os << ",\"unparsed\":" << e.unparsed;
        }
        mem_os << ",\"vers\":" << e.request.vers;
        mem_os << ",\"version_id\":" << e.version_id;

        mem_os << "}" <<endl;
    }

    JSONObjectRef MakeJson( const LogAWSEvent & e, bool accepted )
    {
        JSONObjectRef j = JSON::makeObject();

        MakeCmnJson( j, e, accepted );
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
        j -> addValue( "time", JSON::makeString( FormatTime( e.time ) ) );
        j -> addValue( "res_code", ToJsonString( e.res_code ) );
        j -> addValue( "res_len", ToJsonString( e.res_len ) );
        return j;
    }

    AWSToJsonLogLines( ostream &os, const Options& options ) : cmnLogLines( os, options ) {};
};

struct GCPToJsonLogLines : public GCP_LogLines , public cmnLogLines
{
    virtual void unrecognized( const t_str & text )
    {
        if ( mem_options . path_only ) return;
        cmn_unrecognized( text );
    }

    virtual void acceptLine( const LogGCPEvent & e )
    {
        num_accepted ++;
        line_nr ++;

        if ( mem_options . jsonlib )
        {
            print_json( MakeJson( e, true) );
        }
        else
        {
            print_direct(e, true);
        }
    }

    virtual void rejectLine( const LogGCPEvent & e )
    {
        num_rejected ++;
        line_nr ++;

        if ( mem_options . jsonlib )
        {
            print_json( MakeJson( e, false) );
        }
        else
        {
            print_direct(e, false);
        }
    }

    virtual void headerLine()
    {
        num_headers ++;
        line_nr ++;            
    }

    void print_direct ( const LogGCPEvent & e, bool accepted )
    {
        if ( mem_options . path_only )
        {
            if ( e. request . path . n > 0 )
            {
                mem_os << e.request.path << endl;
            }
            return;
        }
        mem_os << "{\"accepted\":" << (accepted ? "true":"false");
        mem_os << ",\"accession\":" << e.request.accession;
        mem_os << ",\"agent\":" << e.agent;
        mem_os << ",\"bucket\":" << e.bucket;
        mem_os << ",\"extension\":" << e.request.extension;
        mem_os << ",\"filename\":" << e.request.filename;
        mem_os << ",\"host\":" << e.host;
        mem_os << ",\"ip\":" << e.ip;
        mem_os << ",\"ip_region\":" << e.ip_region;
        mem_os << ",\"ip_type\":" << e.ip_type;
        if ( mem_options . print_line_nr )
        {
            mem_os << ",\"line_nr\":" << line_nr;
        }
        mem_os << ",\"method\":" << e.request.method;
        mem_os << ",\"operation\":" << e.operation;
        mem_os << ",\"path\":" << e.request.path;
        mem_os << ",\"referer\":" << e.referer;
        mem_os << ",\"request_bytes\":" << e.request_bytes;
        mem_os << ",\"request_id\":" << e.request_id;
        mem_os << ",\"result_bytes\":" << e.result_bytes;
        mem_os << ",\"source\":\"GS\"";
        mem_os << ",\"status\":" << e.status;
        mem_os << ",\"time\":" << e.time;
        mem_os << ",\"time_taken\":" << e.time_taken;
        mem_os << ",\"uri\":" << e.uri;
        if ( !accepted )
        {
            mem_os << ",\"unparsed\":" << e.unparsed;
        }
        mem_os << ",\"vers\":" << e.request.vers;

        mem_os << "}" <<endl;
    }

    JSONObjectRef MakeJson( const LogGCPEvent & e, bool accepted )
    {
        JSONObjectRef j = JSON::makeObject();

        MakeCmnJson( j, e, accepted );
        j -> addValue( "source", JSON::makeString("GS") );
        j -> addValue( "time", JSON::makeInteger( e.time ) );
        j -> addValue( "ip_type", JSON::makeInteger( e.ip_type ) );
        j -> addValue( "ip_region", ToJsonString( e.ip_region ) );
        j -> addValue( "uri", ToJsonString( e.uri ) );
        j -> addValue( "status", JSON::makeInteger( e.status ) );
        j -> addValue( "request_bytes", JSON::makeInteger( e.request_bytes ) );
        j -> addValue( "result_bytes", JSON::makeInteger( e.result_bytes ) );
        j -> addValue( "time_taken", JSON::makeInteger( e.time_taken ) );
        j -> addValue( "host", ToJsonString( e.host ) );
        j -> addValue( "request_id", ToJsonString( e.request_id ) );
        j -> addValue( "operation", ToJsonString( e.operation ) );
        j -> addValue( "bucket", ToJsonString( e.bucket ) );
        return j;
    }

    GCPToJsonLogLines( ostream &os, const Options& options ) : cmnLogLines( os, options ) {};
};


static void handle_on_prem( const Options & options ) 
{
    cerr << "converting on-premise format" << endl;
    OpToJsonLogLines event_receiver( cout, options );
    OP_Parser p( event_receiver, cin );
    p . setDebug( options . parser_debug );
    p . setLineFilter( options . selected_line );
    p . parse();
    if ( options . report )
        event_receiver.report();
}

static void handle_aws( const Options & options )
{
    cerr << "converting AWS format" << endl;
    AWSToJsonLogLines event_receiver( cout, options );
    AWS_Parser p( event_receiver, cin );
    p . setDebug( options . parser_debug );
    p . setLineFilter( options . selected_line );
    p . parse();
    if ( options . report )
        event_receiver.report();
}

static void handle_gcp( const Options & options ) 
{
    cerr << "converting GCP format" << endl;
    GCPToJsonLogLines event_receiver( cout, options );
    GCP_Parser p( event_receiver, cin );
    p . setDebug( options . parser_debug );
    p . setLineFilter( options . selected_line );
    p . parse();
    if ( options . report )
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

static int perform_parsing( logformat fmt, const Options & options ) 
{
    switch( fmt )
    {
        case op  : handle_on_prem( options ); break;
        case aws : handle_aws( options ); break;
        case gcp : handle_gcp( options ); break;
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
        Options options;
        bool vers = false;
        bool no_report = false;

        ncbi::Cmdline args( argc, argv );
        args . addOption( options . readable, "r", "readable", "pretty print json output ( only with -j )" );
        args . addOption( options . parser_debug, "d", "debug", "run with parser-debug-output" );
        args . addOption( no_report, "n", "no-report", "supress report" );
        args . addOption( options . print_line_nr, "p", "print-line-nr", "print line numbers" );
        args . addOption( options . jsonlib, "j", "jsonlib", "use Json library for output ( much slower )" );
        args . addOption( options . path_only, "a", "path-only", "print only the path found ( not json )" );

        args . addOption( options . selected_line, nullptr, "l", "line-to-select", "line-nr", "select only this line from input (1-based)" );

        args . addOption( vers, "V", "version", "show version" );
        args . addOption( help, "h", "help", "show help" );

        args . startOptionalParams();
        args . addParam( format, "format", "set the input format [ op, aws, gcp ] - dflt:op" );

        args . parse();

        options.report = ! no_report;

        if ( help )
            args . help();

        if ( options . readable && ! options . jsonlib )
        {
            cout << "-r can only be used with -j" << endl;
            return 3;
        }

        if ( vers )
            cout << "version: 1.0" << endl;
        
        if ( !help && !vers )
            return perform_parsing( string2logformat( format ), options );
        else
            return 0;
    }
    catch( const exception & e)
    {
        cerr << "Exception caught: " << e.what() << endl;
        return 1;
    }
    catch( const ncbi::InvalidArgument & e)
    {
        cerr << "Invalid Argument: " << e.what() << endl;
        return 1;
    }
    catch(...)
    {
        cerr << "Unknown exception caught" << endl;
        return 2;
    }
    return 0;
}
