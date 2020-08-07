#pragma once

#include "LogLinesInterface.hpp"
#include <atomic>

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

        class OPReceiverFactory : public ReceiverFactoryInterface
        {
        public:
            virtual ~OPReceiverFactory() {}

            virtual std::unique_ptr<LogLinesInterface> MakeReceiver() const
            {
                std::unique_ptr<FormatterInterface> fmt = std::make_unique<JsonLibFormatter>();
                return std::make_unique<LogOPEvent>( fmt );     
            }
        };

        class OPParser : public ParserInterface
        {
        public:
            OPParser( std::istream & input,  
                       CatWriterInterface & outputs );

            virtual void parse();

        private:
            std::istream & m_input;
            OPReceiverFactory m_factory;
        };

        class OPMultiThreadedParser : public ParserInterface
        {
        public:
            OPMultiThreadedParser( 
                FILE * input,  // need for speed
                CatWriterInterface & outputs,
                size_t queueLimit,
                size_t threadNum
            );

            virtual void parse();
            size_t num_feed_sleeps = 0;
            static std::atomic< size_t > thread_sleeps;

        private:
            FILE * m_input;
            size_t m_queueLimit;
            size_t m_threadNum;
            OPReceiverFactory m_factory;
        };

    }
}

