#pragma once

#include <iostream>
#include <cstring>

#include "types.h"

namespace NCBI
{
    namespace Logging
    {
        typedef enum { acc_before = 0, acc_inside, acc_after } eAccessionMode;

        typedef struct t_request
        {
            t_str method;
            t_str path;
            t_str vers;
            t_str accession;
            t_str filename;
            t_str extension;

            t_str server; // only used in OP

            eAccessionMode accession_mode; // private to the GCP and AWS parser
        } t_request;

        inline void InitRequest( t_request & r ) { memset( & r, 0, sizeof ( r ) ); }

        typedef struct t_agent
        {
            t_str       original;

            t_str       vdb_os;
            t_str       vdb_tool;
            t_str       vdb_release;
            t_str       vdb_phid_compute_env;
            t_str       vdb_phid_guid;
            t_str       vdb_phid_session_id;
            t_str       vdb_libc;     
        } t_agent;

        inline void InitAgent( t_agent & r ) { memset( & r, 0, sizeof ( r ) ); }

        struct CommonLogEvent
        {
            t_str       ip;
            t_str       referer;
            t_agent     agent;
            t_str       unparsed;
            t_request   request;

            CommonLogEvent()
            {
                EMPTY_TSTR( ip );
                EMPTY_TSTR( referer );
                InitAgent( agent );
                EMPTY_TSTR( unparsed );
                InitRequest( request );
            }
        };

#if 0
        struct LogAWSEvent : public CommonLogEvent
        {
            t_str       owner;
            t_str       bucket;
            t_timepoint time;
            t_str       requester;
            t_str       request_id;
            t_str       operation;
            t_str       key;
            t_str       res_code;
            t_str       error;
            t_str       res_len;
            t_str       obj_size;
            t_str       total_time;
            t_str       turnaround_time;
            t_str       version_id;
            t_str       host_id;
            t_str       sig_ver;
            t_str       cipher_suite;
            t_str       auth_type;
            t_str       host_header;
            t_str       tls_version;

            LogAWSEvent()
            {
                EMPTY_TSTR( owner );
                EMPTY_TSTR( bucket );
                memset( &time, 0, sizeof time );
                EMPTY_TSTR( requester );
                EMPTY_TSTR( request_id );
                EMPTY_TSTR( operation );
                EMPTY_TSTR( key );
                EMPTY_TSTR( res_code );
                EMPTY_TSTR( error );
                EMPTY_TSTR( res_len );
                EMPTY_TSTR( obj_size );
                EMPTY_TSTR( total_time );
                EMPTY_TSTR( turnaround_time );
                EMPTY_TSTR( version_id );
                EMPTY_TSTR( host_id );
                EMPTY_TSTR( sig_ver );
                EMPTY_TSTR( cipher_suite );
                EMPTY_TSTR( auth_type );
                EMPTY_TSTR( host_header );
                EMPTY_TSTR( tls_version );
            }
        };
#endif

        struct LogLinesInterface
        {
            virtual ~LogLinesInterface() {}

            virtual void beginLine() = 0;
            virtual void endLine() = 0;

            enum { cat_review, cat_good, cat_bad, cat_ugly } Category;

            virtual Category GetCategory() const = 0;

            // LogLinesInterface();
            // virtual LogLinesInterface();

            virtual void failedToParse( const t_str & source ) = 0;

            typedef enum { 
                ip,
                referer,
                agent,
                unparsed,
                request,                
                LastMemberId = request
            } Members;

            virtual void set( Members m, const t_str & v ) = 0;
            virtual void set( Members m, int64_t v ) = 0;
            virtual void set( const t_agent & a ) = 0;
            virtual void set( const t_request & r ) = 0;

            class Member
            {
                t_str name;
                enum { t_string, t_int } Type;
                Type type;
                union 
                {
                    t_str s;
                    int64_t i;
                } u;
            };
            virtual bool nextMember( Member & value ) = 0;

            // could be a throw instead of abort, but we need to make sure the parser terminates cleanly if we throw.
            typedef enum { proceed, abort } ReportFieldResult;
            typedef enum { suspect, bad } ReportFieldType;
            virtual ReportFieldResult reportField( Members field, const char * value, const char * message, ReportFieldType type ) = 0;
        };
    }
}

