#pragma once

#include "ReceiverInterface.hpp"
#include "ParserInterface.hpp"

#include <memory>

namespace NCBI
{
    namespace Logging
    {
        struct GCPReceiver : public ReceiverInterface
        {
            using ReceiverInterface::set;

            GCPReceiver( ReceiverInterface::FormatterRef fmt );

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
            virtual void set( GCP_Members m, const t_str & v ); // will invoke set( ReceiverInterface::Members ) if necessary

        private:
        };

        class GCPParseBlockFactory : public ParseBlockFactoryInterface
        {
        public:
            virtual ~GCPParseBlockFactory();
            virtual std::unique_ptr<ParseBlockInterface> MakeParseBlock() const;
        };

        class GCPReverseBlockFactory : public ParseBlockFactoryInterface
        {
        public:
            virtual ~GCPReverseBlockFactory();
            virtual std::unique_ptr<ParseBlockInterface> MakeParseBlock() const;
        };

    }
}

