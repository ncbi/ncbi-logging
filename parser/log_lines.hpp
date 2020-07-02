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

        std::string ToString( const t_timepoint& t ); // [dd/Mon/yyyy:hh:mm:ss (-|+)oooo]

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

        struct CommonLogEvent
        {
            t_str       ip;
            t_str       referer;
            t_str       agent;
            t_str       unparsed;
            t_request   request;

            CommonLogEvent()
            {
                EMPTY_TSTR( ip );
                EMPTY_TSTR( referer );
                EMPTY_TSTR( agent );
                EMPTY_TSTR( unparsed );
                InitRequest( request );
            }
        };

        // Maybe in a separate header
        struct LogOPEvent : public CommonLogEvent
        {
            t_timepoint time;
            t_str       user;
            t_str       req_time;
            t_str       forwarded;
            int64_t     port;
            int64_t     req_len;

            int64_t     res_code;
            int64_t     res_len;

            LogOPEvent() : CommonLogEvent()
            {
                memset( &time, 0, sizeof time );
                EMPTY_TSTR( user );
                EMPTY_TSTR( req_time );
                EMPTY_TSTR( forwarded );
                port  = 0;
                req_len = 0;
                res_code = 0;
                res_len = 0;
            }
        };

        // Maybe in a separate header
        struct LogGCPEvent : public CommonLogEvent
        {
            int64_t     time;           // in microseconds
            int64_t     ip_type;        // 1..ipv4, 2..ipv6
            t_str       ip_region;      // empty, reserved
            t_str       uri;
            int64_t     status;
            int64_t     request_bytes;
            int64_t     result_bytes;
            int64_t     time_taken;     // in microsenconds
            t_str       host;
            t_str       request_id;
            t_str       operation;
            t_str       bucket;

            LogGCPEvent()
            {
                time = 0;
                ip_type = 0;
                EMPTY_TSTR( ip_region );
                EMPTY_TSTR( uri );
                status = 0;
                request_bytes = 0;
                result_bytes = 0;
                time_taken = 0;
                EMPTY_TSTR( host );
                EMPTY_TSTR( request_id );
                EMPTY_TSTR( operation );
                EMPTY_TSTR( bucket );
            }
        };

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
                EMPTY_TSTR( cipher_suite );
                EMPTY_TSTR( auth_type );
                EMPTY_TSTR( host_header );
                EMPTY_TSTR( tls_version );
            }
        };

        struct OP_LogLines
        {
            virtual void unrecognized( const t_str & text ) = 0;

            // any t_str structures incide event will be invalidated upon return from these methods
            // if you want to hang on to the strings, copy them inside here
            virtual void acceptLine( const LogOPEvent & event ) = 0;
            virtual void rejectLine( const LogOPEvent & event ) = 0;

            virtual ~ OP_LogLines () noexcept {}
        };

        struct AWS_LogLines
        {
            virtual void unrecognized( const t_str & text ) = 0;

            // any t_str structures incide event will be invalidated upon return from these methods
            // if you want to hang on to the strings, copy them inside here
            virtual void acceptLine( const LogAWSEvent & event ) = 0;
            virtual void rejectLine( const LogAWSEvent & event ) = 0;

            virtual ~ AWS_LogLines () noexcept {}
        };

        struct GCP_LogLines
        {
            virtual void unrecognized( const t_str & text ) = 0;

            // any t_str structures incide event will be invalidated upon return from these methods
            // if you want to hang on to the strings, copy them inside here
            virtual void acceptLine( const LogGCPEvent & event ) = 0;
            virtual void rejectLine( const LogGCPEvent & event ) = 0;
            virtual void headerLine() = 0;

            virtual ~ GCP_LogLines () noexcept {}
        };


        class OP_Parser
        {
        public:
            OP_Parser( OP_LogLines &loglines, std::istream &input );

            void parse(); 

            void setDebug( bool on );
            void setLineFilter( unsigned long int line_nr );

        private:
            OP_LogLines &   m_lines;
            std::istream &  m_input;
            unsigned long int line_filter = 0;
        };

        class AWS_Parser
        {
        public:
            AWS_Parser( AWS_LogLines &loglines, std::istream &input );

            void parse(); 

            void setDebug( bool on );
            void setLineFilter( unsigned long int line_nr );

        private:
            AWS_LogLines &  m_lines;
            std::istream &  m_input;
            unsigned long int line_filter = 0;            
        };

        class GCP_Parser
        {
        public:
            GCP_Parser( GCP_LogLines &loglines, std::istream &input );

            void parse(); 

            void setDebug( bool on );
            void setLineFilter( unsigned long int line_nr );

        private:
            GCP_LogLines &  m_lines;
            std::istream &  m_input;
            unsigned long int line_filter = 0;
        };

    }
}

