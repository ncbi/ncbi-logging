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

        enum e_format
        {
            e_onprem, e_aws, e_gcp
        };

        struct CommonLogEvent
        {
            e_format    m_format;

            t_str       ip;
            t_timepoint time;
            t_request   request;
            int64_t     res_code;
            int64_t     res_len;
            t_str       referer;
            t_str       agent;

            t_str       unparsed;

            CommonLogEvent( e_format format ) : m_format( format )
            {
                ip . n = 0;
                referer . n = 0;
                agent . n = 0;
                unparsed . n = 0;
                memset( &request, 0, sizeof request );
            }
        };

        struct LogOnPremEvent : public CommonLogEvent
        {
            t_str       user;
            t_str       req_time;
            t_str       forwarded;
            int64_t     port;
            int64_t     req_len;

            LogOnPremEvent() : CommonLogEvent( e_onprem )
            {
                user . n = 0;
                req_time . n = 0;
                forwarded . n = 0;
            }
        };

        struct LogAWSEvent : public CommonLogEvent
        {
            t_str       owner;
            t_str       bucket;
            t_str       requester;
            t_str       request_id;
            t_str       operation;
            t_str       key;
            t_str       error;
            int64_t     obj_size;
            int64_t     total_time;
            t_str       version_id;
            t_str       host_id;
            t_str       cipher_suite;
            t_str       auth_type;
            t_str       host_header;
            t_str       tls_version;

            LogAWSEvent() : CommonLogEvent( e_aws )
            {
                owner . n = 0;
                bucket . n = 0;
                requester . n = 0;
                request_id . n = 0;
                operation . n = 0;
                key . n = 0;
                error . n = 0;
                version_id . n = 0;
                host_id . n = 0;
                cipher_suite . n = 0;
                auth_type . n = 0;
                host_header . n = 0;
                tls_version . n = 0;
            }
        };

        struct LogLines
        {
            // TODO maybe using an exception to abort if the receiver
            // cannot handle the events ( any more ) ... 
            virtual int unrecognized( const t_str & text ) = 0;

            virtual int acceptLine( const CommonLogEvent & event ) = 0;
            virtual int rejectLine( const CommonLogEvent & event ) = 0;

            virtual ~ LogLines () noexcept {}
        };

        class LogParser
        {
        public:
            LogParser( LogLines &, std::istream & );
            LogParser( LogLines & ); // uses cin for input

            bool parse();

            void setDebug( bool on );

        private:
            LogLines &      m_lines;
            std::istream &  m_input;
        };

    }
}

