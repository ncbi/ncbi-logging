#pragma once

#include <iostream>

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

        struct LogOnPremEvent
        {
            t_str       ip;
            t_str       user;
            t_timepoint time;
            t_request   request;
            int64_t     res_code;
            int64_t     res_len;
            t_str       req_time;
            t_str       referer;
            t_str       agent;
            t_str       forwarded;
            int64_t     port;
            int64_t     req_len;

            t_str       unparsed;
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
            int64_t     status;
            t_str       error;
            int64_t     bytes_sent;
            int64_t     obj_size;
            int64_t     total_time;
            t_str       referer;
            t_str       agent;
            t_str       version_id;
            t_str       host_id;
            t_str       cipher_suite;
            t_str       auth_type;
            t_str       host_header;
            t_str       tls_version;

            t_str       unparsed;
        };

        struct LogLines
        {
            // TODO maybe using an exception to abort if the receiver
            // cannot handle the events ( any more ) ... 
            virtual int unrecognized( const t_str & text ) = 0;

            virtual int acceptLine( const LogOnPremEvent & event ) = 0;
            virtual int acceptLine( const LogAWSEvent & event ) = 0;

            virtual int rejectLine( const LogOnPremEvent & event ) = 0;
            virtual int rejectLine( const LogAWSEvent & event ) = 0;

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

