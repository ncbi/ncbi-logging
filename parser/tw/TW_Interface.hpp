#pragma once

#include "ReceiverInterface.hpp"

#include <memory>

namespace NCBI
{
    namespace Logging
    {
        struct TWReceiver : public ReceiverInterface
        {
            using ReceiverInterface::set;
            
            TWReceiver( std::unique_ptr<FormatterInterface> & fmt );

            typedef enum { 
                id1 = LastMemberId+1,
                id2,
                id3,
                time,
                server,
                sid,
                service,
                event,
                msg,
                TW_LastMemberId = msg
            } TW_Members; // all are t_str values
            virtual void set( TW_Members m, const t_str & ); // will invoke set( ReceiverInterface::Members ) if necessary

        private:
        };

        class TWParseBlockFactory : public ParseBlockFactoryInterface
        {
        public:
            virtual ~TWParseBlockFactory();
            virtual std::unique_ptr<ParseBlockInterface> MakeParseBlock() const;
        };


    }
}

