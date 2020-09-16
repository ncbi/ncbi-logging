#pragma once

#include "ReceiverInterface.hpp"

#include <memory>

namespace NCBI
{
    namespace Logging
    {
        struct OPReceiver : public ReceiverInterface
        {
            using ReceiverInterface::set;
            
            OPReceiver( std::unique_ptr<FormatterInterface> & fmt );

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

        private:
        };

        class OPParseBlockFactory : public ParseBlockFactoryInterface
        {
        public:
            virtual ~OPParseBlockFactory();
            virtual std::unique_ptr<ParseBlockInterface> MakeParseBlock() const;
        };


    }
}

