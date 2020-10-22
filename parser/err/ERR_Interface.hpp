#pragma once

#include "ReceiverInterface.hpp"
#include "ParserInterface.hpp"

#include <memory>

namespace NCBI
{
    namespace Logging
    {
        struct ERRReceiver : public ReceiverInterface
        {
            using ReceiverInterface::set;

            ERRReceiver( ReceiverInterface::FormatterRef fmt );

            typedef enum {
                datetime = LastMemberId+1,
                severity,
                pid,
                msg,
                TW_LastMemberId = msg
            } ERR_Members; // all are t_str values
            virtual void set( ERR_Members m, const t_str & ); // will invoke set( ReceiverInterface::Members ) if necessary

            virtual Category post_process( void );

            std::string msg_for_postprocess;
        };

        class ERRParseBlockFactory : public ParseBlockFactoryInterface
        {
        public:
            virtual ~ERRParseBlockFactory();
            virtual std::unique_ptr<ParseBlockInterface> MakeParseBlock() const;
        };

        class ERRReverseBlockFactory : public ParseBlockFactoryInterface
        {
        public:
            virtual ~ERRReverseBlockFactory();
            virtual std::unique_ptr<ParseBlockInterface> MakeParseBlock() const;
        };

    }
}

