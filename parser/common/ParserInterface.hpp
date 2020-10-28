#pragma once

#include <memory>

namespace ncbi
{
    class JSONObject;
}

namespace NCBI
{
    namespace Logging
    {
        class ReceiverInterface;
        class CatWriterInterface;
        class LineSplitterInterface;
        class ParserDriverInterface;

        class ParseBlockInterface
        {
        public:
            ParseBlockInterface() : m_debug( false ) {}
            virtual ~ParseBlockInterface() = 0;
            virtual ReceiverInterface & GetReceiver() = 0;

            // this function is responsible to set the category of it's receiver
            // the return value is the success of the parser
            // if return-value is false -> the category will be cat_ugly
            virtual bool format_specific_parse( const char * line, size_t line_size ) = 0;

            void SetDebug( bool onOff ) { m_debug = onOff; }

            void receive_one_line( const char * line, size_t line_size, size_t line_nr );
            bool m_debug;
        };

        class ParseBlockFactoryInterface
        {
        public:
            ParseBlockFactoryInterface();
            virtual ~ParseBlockFactoryInterface() = 0;

            void setFast( bool onOff ) { m_fast = onOff; }
            void setNumThreads( size_t num_threads ) { m_nthreads = num_threads; }

            virtual std::unique_ptr<ParseBlockInterface> MakeParseBlock() const = 0;

            std::unique_ptr<ParserDriverInterface> MakeParserDriver(
                LineSplitterInterface & input, CatWriterInterface & output );

            bool m_fast;
            size_t m_nthreads;
        };

        class ParserDriverInterface
        {
        public :
            virtual ~ParserDriverInterface();
            virtual void parse_all_lines() = 0;

        protected :
            ParserDriverInterface( LineSplitterInterface & input,
                    CatWriterInterface & outputs );

            LineSplitterInterface & m_input;
            CatWriterInterface & m_outputs;
        };

        class SingleThreadedDriver : public ParserDriverInterface
        {
        public:
            SingleThreadedDriver( LineSplitterInterface & input,
                    CatWriterInterface & outputs,
                    std::unique_ptr<ParseBlockInterface> pb );

            virtual void parse_all_lines();
            void setDebug ( bool onOff ) { m_debug = onOff; }

            // for testing
            std::unique_ptr<ParseBlockInterface> getParseBlock()
            {
                return std::move( m_pb );
            }

        protected:
            bool m_debug;
            std::unique_ptr<ParseBlockInterface> m_pb;
        };

        class MultiThreadedDriver : public ParserDriverInterface
        {
        public:
            MultiThreadedDriver(
                LineSplitterInterface & input,
                CatWriterInterface & outputs,
                size_t queueLimit,
                size_t threadNum,
                ParseBlockFactoryInterface & pbFact
            );

            virtual void parse_all_lines();

        private:
            size_t m_queueLimit;
            size_t m_threadNum;
            ParseBlockFactoryInterface & m_pbFact;
        };

    }
}

