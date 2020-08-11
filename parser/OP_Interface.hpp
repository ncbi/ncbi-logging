#pragma once

#include "LogLinesInterface.hpp"

#include <memory>

namespace NCBI
{
    namespace Logging
    {
        struct LogOPEvent : public LogLinesInterface
        {
            using LogLinesInterface::set;
            
            LogOPEvent( std::unique_ptr<FormatterInterface> & fmt );

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
            virtual void set( OP_Members m, const t_str & ); // will invoke set( LogLinesInterface::Members ) if necessary

            virtual void reportField( const char * message );

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

