#pragma once

#include "LogLinesInterface.hpp"

#include <memory>

namespace NCBI
{
    namespace Logging
    {
        struct LogAWSEvent : public LogLinesInterface
        {
            using LogLinesInterface::set;
            
            LogAWSEvent( std::unique_ptr<FormatterInterface> & fmt );

            typedef enum { 
                owner = LastMemberId+1,
                bucket,
                time,
                requester,
                request_id,
                operation,
                key,
                res_code,
                error,
                res_len,
                obj_size,
                total_time,
                turnaround_time,
                version_id,
                host_id,
                sig_ver,
                cipher_suite,
                auth_type,
                host_header,
                tls_version,
                AWS_LastMemberId = tls_version
            } AWS_Members; // all are t_str values
            virtual void set( AWS_Members m, const t_str & ); // will invoke set( LogLinesInterface::Members ) if necessary

            virtual void reportField( const char * message );

        private:
        };

        class AWSParseBlockFactory : public ParseBlockFactoryInterface
        {
        public:
            AWSParseBlockFactory( bool fast ) : ParseBlockFactoryInterface( fast ) {}
            virtual ~AWSParseBlockFactory();
            virtual std::unique_ptr<ParseBlockInterface> MakeParseBlock() const;
        };

    }
}

