#pragma once

#include <iostream>
#include <sstream>
#include <iomanip>

#include "helper.hpp"

#include "AWS_Interface.hpp"

namespace NCBI
{
    namespace Logging
    {
        struct Options
        {
            bool print_line_nr = false;
            bool parser_debug = false;
            bool path_only = false;
            bool agent_only = false;
            unsigned long int selected_line = 0;
        };

        struct AWSToJsonLogLines : public AWS_LogLinesInterface
        {
            AWSToJsonLogLines( std :: ostream &os, const Options& options ) {};

            virtual ~AWSToJsonLogLines() {}

            virtual void failedToParse( const t_str & source ) {}

            virtual void beginLine() {}
            virtual void endLine(  ) {}

            virtual void set( Members, const t_str & ){}           //{ switch( m ) { case ip: ...; default: throw; }; }
            virtual void set( Members, int64_t ){}
            virtual void set( const t_agent & a ){}
            virtual void set( const t_request & r ){}
            virtual void set( AWS_Members m, const t_str & ){}   //{ switch( m ) { case owner...; default: set(Members(m)) }; }

            // could be a throw instead of abort, but we need to make sure the parser terminates cleanly if we throw.
            virtual ReportFieldResult reportField( Members field, const char * value, const char * message, ReportFieldType type ) { return abort; }
        };

#if 0
struct AWSToJsonLogLines : public AWS_LogLines , public cmnLogLines
{
    virtual void unrecognized( const t_str & text )
    {
        if ( mem_options . path_only || mem_options . agent_only ) return;
        cmn_unrecognized( text );
    }

    virtual void acceptLine( const LogAWSEvent & e )
    {
        num_accepted ++;
        line_nr ++;
        print_direct(e, true );
    }

    virtual void rejectLine( const LogAWSEvent & e )
    {
        num_rejected ++;
        line_nr ++;
        print_direct(e, false);
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
        else if ( mem_options . agent_only )
        {
            if ( e. agent . original . n > 0 )
            {
                mem_os << e.agent . original << endl;
            }
            return;
        }

        mem_os << "{\"accepted\":" << (accepted ? "true":"false");
        mem_os << ",\"accession\":" << e.request.accession;
        mem_os << ",\"agent\":" << e.agent . original;
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
        mem_os << ",\"sig_ver\":" << e.sig_ver;
        mem_os << ",\"source\":\"S3\"";
        mem_os << ",\"time\":" << e.time;
        mem_os << ",\"tls_version\":" << e.tls_version;
        mem_os << ",\"total_time\":" << e.total_time;
        mem_os << ",\"turnaround_time\":" << e.turnaround_time;
        if ( !accepted )
        {
            mem_os << ",\"unparsed\":" << e.unparsed;
        }

        mem_os << ",\"vdb_libc\":" << e.agent.vdb_libc;
        mem_os << ",\"vdb_os\":" << e.agent.vdb_os;
        mem_os << ",\"vdb_phid_compute_env\":" << e.agent.vdb_phid_compute_env;
        mem_os << ",\"vdb_phid_guid\":" << e.agent.vdb_phid_guid;
        mem_os << ",\"vdb_phid_session_id\":" << e.agent.vdb_phid_session_id;
        mem_os << ",\"vdb_release\":" << e.agent.vdb_release;
        mem_os << ",\"vdb_tool\":" << e.agent.vdb_tool;

        mem_os << ",\"vers\":" << e.request.vers;
        mem_os << ",\"version_id\":" << e.version_id;

        mem_os << "}" <<endl;
    }

    AWSToJsonLogLines( ostream &os, const Options& options ) : cmnLogLines( os, options ) {};
};
#endif

    }
}