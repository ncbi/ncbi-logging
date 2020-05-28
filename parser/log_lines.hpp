#pragma once

#include <iostream>
#include <cstring>

#include "types.h"

namespace NCBI
{
    namespace Logging
    {
        typedef struct t_timepoint
        {
            uint8_t day;
            uint8_t month;
            uint32_t year;
            uint8_t hour;
            uint8_t minute;
            uint8_t second;
            int32_t offset;
        } t_timepoint;

        typedef struct t_request
        {
            t_str server;
            t_str method;
            t_str path;
            t_str params;
            t_str vers;
        } t_request;

        struct CommonLogEvent
        {
            t_str       ip;
            t_timepoint time;
            t_request   request;
            int64_t     res_code;
            int64_t     res_len;
            t_str       referer;
            t_str       agent;

            t_str       unparsed;

            CommonLogEvent()
            {
                ip . n = 0;
                referer . n = 0;
                agent . n = 0;
                unparsed . n = 0;
                memset( &request, 0, sizeof request );
            }
        };

        // Maybe in a separate header
        struct LogOPEvent : public CommonLogEvent
        {
            t_str       user;
            t_str       req_time;
            t_str       forwarded;
            int64_t     port;
            int64_t     req_len;

            LogOPEvent() : CommonLogEvent()
            {
                user . n = 0;
                req_time . n = 0;
                forwarded . n = 0;
            }
        };

        // Maybe in a separate header
        struct LogGCPEvent
        {
            int64_t     time;           // in microseconds
            t_str       ip;
            int64_t     ip_type;        // 1..ipv4, 2..ipv6
            t_str       ip_region;      // empty, reserved
            t_str       method;
            t_str       uri;
            int64_t     status;
            int64_t     request_bytes;
            int64_t     result_bytes;
            int64_t     time_taken;     // in microsenconds
            t_str       host;
            t_str       referrer;
            t_str       agent;
            t_str       request_id;
            t_str       operation;
            t_str       bucket;
            t_str       object;

            LogGCPEvent()
            {
                time = 0;
                ip . n = 0;
                ip_type = 0;
                ip_region . n = 0;
                method . n = 0;
                uri . n = 0;
                status = 0;
                request_bytes = 0;
                result_bytes = 0;
                time_taken = 0;
                host . n = 0;
                referrer . n = 0;
                agent . n = 0;
                request_id . n = 0;
                operation . n = 0;
                bucket . n = 0;
                object . n = 0;
            }
        };

        struct LogAWSEvent
        {
            t_str       owner;
            t_str       bucket;
            t_timepoint time;
            t_str       ip;
            t_str       requester;
            t_str       request_id;
            t_str       operation;
            t_str       key;
            t_request   request;
            t_str       res_code;
            t_str       error;
            t_str       res_len;
            t_str       obj_size;
            t_str       total_time;
            t_str       turnaround_time;
            t_str       referer;
            t_str       agent;
            t_str       version_id;
            t_str       host_id;
            t_str       cipher_suite;
            t_str       auth_type;
            t_str       host_header;
            t_str       tls_version;

            t_str       unparsed;

            LogAWSEvent()
            {
                owner . n = 0;
                bucket . n = 0;
                ip . n = 0;
                requester . n = 0;
                request_id . n = 0;
                operation . n = 0;
                key . n = 0;
                res_code . n = 0;
                error . n = 0;
                res_len . n = 0;
                obj_size . n = 0;
                total_time . n = 0;
                turnaround_time . n = 0;
                referer . n = 0;
                agent . n = 0;
                version_id . n = 0;
                host_id . n = 0;
                cipher_suite . n = 0;
                auth_type . n = 0;
                host_header . n = 0;
                tls_version . n = 0;

                unparsed . n = 0;
            }
        };

        struct OP_LogLines
        {
            // TODO maybe using an exception to abort if the receiver
            // cannot handle the events ( any more ) ...
            // i.e. return void
            virtual int unrecognized( const t_str & text ) = 0;

            // any t_str structures incide event will be invalidated upon return from these methods
            // if you want to hang on to the strings, copy them inside here
            virtual int acceptLine( const LogOPEvent & event ) = 0;
            virtual int rejectLine( const LogOPEvent & event ) = 0;

            virtual ~ OP_LogLines () noexcept {}
        };

        struct AWS_LogLines
        {
            // TODO maybe using an exception to abort if the receiver
            // cannot handle the events ( any more ) ...
            // i.e. return void
            virtual int unrecognized( const t_str & text ) = 0;

            // any t_str structures incide event will be invalidated upon return from these methods
            // if you want to hang on to the strings, copy them inside here
            virtual int acceptLine( const LogAWSEvent & event ) = 0;
            virtual int rejectLine( const LogAWSEvent & event ) = 0;

            virtual ~ AWS_LogLines () noexcept {}
        };

        struct GCP_LogLines
        {
            // TODO maybe using an exception to abort if the receiver
            // cannot handle the events ( any more ) ...
            // i.e. return void
            virtual int unrecognized( const t_str & text ) = 0;

            // any t_str structures incide event will be invalidated upon return from these methods
            // if you want to hang on to the strings, copy them inside here
            virtual int acceptLine( const LogGCPEvent & event ) = 0;
            virtual int rejectLine( const LogGCPEvent & event ) = 0;

            virtual ~ GCP_LogLines () noexcept {}
        };


        class OP_Parser
        {
        public:
            OP_Parser( OP_LogLines &loglines, std::istream &input );
            OP_Parser( OP_LogLines &loglines ); // uses cin for input

            bool parse(); // maybe void and rely on exceptions to communicate "big" failures

            void setDebug( bool on );

        private:
            OP_LogLines &   m_lines;
            std::istream &  m_input;
        };

        class AWS_Parser
        {
        public:
            AWS_Parser( AWS_LogLines &loglines, std::istream &input );
            AWS_Parser( AWS_LogLines &loglines ); // uses cin for input

            bool parse(); // maybe void and rely on exceptions to communicate "big" failures

            void setDebug( bool on );

        private:
            AWS_LogLines &  m_lines;
            std::istream &  m_input;
        };

        class GCP_Parser
        {
        public:
            GCP_Parser( GCP_LogLines &loglines, std::istream &input );
            GCP_Parser( GCP_LogLines &loglines ); // uses cin for input

            bool parse(); // maybe void and rely on exceptions to communicate "big" failures

            void setDebug( bool on );

        private:
            GCP_LogLines &  m_lines;
            std::istream &  m_input;
        };


    }
}

