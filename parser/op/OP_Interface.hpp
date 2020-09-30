#pragma once

#include "ReceiverInterface.hpp"
#include "ParserInterface.hpp"

#include <memory>

namespace NCBI
{
    namespace Logging
    {
        struct OPReceiver : public ReceiverInterface
        {
            using ReceiverInterface::set;

            OPReceiver( ReceiverInterface::FormatterRef fmt );

            typedef enum {
                owner = LastMemberId+1,
                time,
                server,
                user,
                req_time,
                forwarded,
                port,
                req_len,
                res_code,
                res_len,
                OP_LastMemberId = res_len
            } OP_Members; // all are t_str values
            virtual void set( OP_Members m, const t_str & ); // will invoke set( ReceiverInterface::Members ) if necessary

            virtual void post_process( void );

            std::string agent_for_postprocess;
        };

        class OPParseBlockFactory : public ParseBlockFactoryInterface
        {
        public:
            virtual ~OPParseBlockFactory();
            virtual std::unique_ptr<ParseBlockInterface> MakeParseBlock() const;
        };

        class OPReverseBlockFactory : public ParseBlockFactoryInterface
        {
        public:
            virtual ~OPReverseBlockFactory();
            virtual std::unique_ptr<ParseBlockInterface> MakeParseBlock() const;
        };

    }
}

