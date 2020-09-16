#pragma once

#include <string>
#include <cstring>
#include <memory>
#include <stdexcept>

#include "types.h"
#include "Formatters.hpp"

namespace NCBI
{
    namespace Logging
    {
        class ReceiverInterface;
        class CatWriterInterface;
        class LineSplitterInterface;
        class ParserDriverInterface;

        typedef enum { acc_before = 0, acc_inside, acc_after } eAccessionMode;

        typedef struct t_request
        {
            t_str method;
            t_str path;
            t_str vers;
            t_str accession;
            t_str filename;
            t_str extension;

            eAccessionMode accession_mode; // private to the GCP and AWS parser
        } t_request;

        inline void InitRequest( t_request & r ) { memset( & r, 0, sizeof ( r ) ); }

        typedef struct t_agent
        {
            t_str       original;

            t_str       vdb_os;
            t_str       vdb_tool;
            t_str       vdb_release;
            t_str       vdb_phid_compute_env;
            t_str       vdb_phid_guid;
            t_str       vdb_phid_session_id;
            t_str       vdb_libc;
        } t_agent;

        inline void InitAgent( t_agent & r ) { memset( & r, 0, sizeof ( r ) ); }

        struct ReceiverInterface
        {
            ReceiverInterface( std::unique_ptr<FormatterInterface> & p_fmt );

            virtual ~ReceiverInterface();

            typedef enum {
                cat_review = 0,
                cat_good,
                cat_bad,
                cat_ugly,
                cat_ignored,
                cat_unknown,
                cat_max } Category;

            Category GetCategory() const { return m_cat; }
            void SetCategory( Category p_cat ) { m_cat = p_cat; }

            typedef enum {
                ip,
                referer,
                unparsed,
                agent,
                request,
                LastMemberId = unparsed
            } Members;

            void set( Members m, const t_str & v ); // for agent and request, use the methods below
            void setAgent( const t_agent & a );
            void setRequest( const t_request & r );

            void setMember( const char * mem, const t_str & v );

            void reportField( const char * message );

            FormatterInterface & GetFormatter()
            {
                FormatterInterface * p = m_fmt.get();
                if ( nullptr != p )
                    return * p;
                throw std::logic_error( "no formatter available" );
            }

        protected:
            std::unique_ptr<FormatterInterface> m_fmt;
            Category m_cat;
        };

        class ParseBlockInterface
        {
        public:
            virtual ~ParseBlockInterface() = 0;
            virtual ReceiverInterface & GetReceiver() = 0;
            virtual bool format_specific_parse( const char * line, size_t line_size ) = 0;
            virtual void SetDebug( bool onOff ) = 0;

            void receive_one_line( const char * line, size_t line_size, size_t line_nr );
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

