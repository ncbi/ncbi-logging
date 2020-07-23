#pragma once

#include <string>
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

        struct LogLinesInterface
        {
            virtual ~LogLinesInterface() {}

            virtual void beginLine() = 0;
            virtual void endLine() = 0;

            typedef enum { cat_review, cat_good, cat_bad, cat_ugly } Category;

            virtual Category GetCategory() const = 0;

            // LogLinesInterface();
            // virtual LogLinesInterface();

            virtual void failedToParse( const t_str & source ) = 0;

            typedef enum { 
                ip,
                referer,
                unparsed,
                LastMemberId = unparsed
            } Members;

            virtual void set( Members m, const t_str & v ) = 0;
            virtual void set( Members m, int64_t v ) = 0;
            virtual void setAgent( const t_agent & a ) = 0;
            virtual void setRequest( const t_request & r ) = 0;

            typedef struct Member
            {
                t_str name;
                typedef enum { t_string, t_int, t_time } Type;
                Type type;
                union 
                {
                    t_str s;
                    int64_t i;
                    t_timepoint t;
                } u;
            } Member;
            virtual bool nextMember( Member & value ) = 0;

            // could be a throw instead of abort, but we need to make sure the parser terminates cleanly if we throw.
            typedef enum { proceed, abort } ReportFieldResult;
            typedef enum { suspect, bad } ReportFieldType;
            virtual ReportFieldResult reportField( Members field, const char * value, const char * message, ReportFieldType type ) = 0;
        };

    }
}

