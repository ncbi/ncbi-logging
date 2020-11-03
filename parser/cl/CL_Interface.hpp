#pragma once

#include "ReceiverInterface.hpp"
#include "ParserInterface.hpp"

#include <memory>

namespace NCBI
{
    namespace Logging
    {
        struct CLReceiver : public ReceiverInterface
        {
            using ReceiverInterface::set;

            CLReceiver( ReceiverInterface::FormatterRef fmt );

            typedef enum {
                syslog_prefix = LastMemberId+1,
                timestamp,
                owner,
                bucket,
                contentAccessorUserID,
                requestHdrSize,
                requestBodySize,
                responseHdrSize,
                responseBodySize,
                totalSize,
                durationMsec,
                httpStatus,
                reqId,
                eTag,
                errorCode,
                copySource,
                CL_LastMemberId = copySource
            } CL_Members; // all are t_str values
            virtual void set( CL_Members m, const t_str & ); // will invoke set( ReceiverInterface::Members ) if necessary

            virtual Category post_process( void );

            std::string obj_for_postprocess;
        };

        class CLParseBlockFactory : public ParseBlockFactoryInterface
        {
        public:
            virtual ~CLParseBlockFactory();
            virtual std::unique_ptr<ParseBlockInterface> MakeParseBlock() const;
        };

        class CLReverseBlockFactory : public ParseBlockFactoryInterface
        {
        public:
            virtual ~CLReverseBlockFactory();
            virtual std::unique_ptr<ParseBlockInterface> MakeParseBlock() const;
        };

    }
}
