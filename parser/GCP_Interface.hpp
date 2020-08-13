#pragma once

#include "LogLinesInterface.hpp"

#include <memory>

namespace NCBI
{
    namespace Logging
    {
        struct LogGCPEvent : public LogLinesInterface
        {
            using LogLinesInterface::set;
            
            LogGCPEvent( std::unique_ptr<FormatterInterface> & fmt );

            typedef enum { 
                time = LastMemberId+1,
                ip_type,
                ip_region,
                uri,
                status,
                request_bytes,
                result_bytes,
                time_taken,
                host,
                request_id,
                operation,
                bucket,
                GCP_LastMemberId = bucket
            } GCP_Members; // all are t_str values
            virtual void set( GCP_Members m, const t_str & v ); // will invoke set( LogLinesInterface::Members ) if necessary

            virtual void reportField( const char * message );

        private:
        };

        class GCPParseBlockFactory : public ParseBlockFactoryInterface
        {
        public:
            GCPParseBlockFactory( bool fast ) : ParseBlockFactoryInterface( fast ) {}
            virtual ~GCPParseBlockFactory();
            virtual std::unique_ptr<ParseBlockInterface> MakeParseBlock() const;
        };


    }
}

