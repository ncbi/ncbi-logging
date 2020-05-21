#pragma once

#include <string>
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

        struct LogEvent
        {
            std::string ip;
            std::string user;
            t_timepoint time;
            t_request   request;
            int64_t     res_code;
            int64_t     res_len;
            std::string req_time;
            std::string referer;
            std::string agent;
            std::string forwarded;
            int64_t     port;
            int64_t     req_len;

            std::string unparsed;
        };

        struct LogLines
        {
            virtual int unrecognized( const std::string & text ) = 0;
            virtual int acceptLine( const LogEvent & event ) = 0;
            virtual int rejectLine( const LogEvent & event ) = 0;

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

