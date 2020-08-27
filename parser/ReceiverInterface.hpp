#pragma once

#include <string>
#include <cstring>
#include <memory>
#include <atomic>

#include "types.h"
#include "Formatters.hpp"

namespace NCBI
{
    namespace Logging
    {
        class ReceiverInterface;
        class CatWriterInterface;

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

            FormatterInterface & GetFormatter() { return * m_fmt.get(); }

        protected:
            std::unique_ptr<FormatterInterface> m_fmt;
            Category m_cat;
        };

        class ParseBlockInterface
        {
        public:
            virtual ~ParseBlockInterface() = 0;
            virtual ReceiverInterface & GetReceiver() = 0;
            virtual bool Parse( const std :: string & line ) = 0;
            virtual void SetDebug( bool onOff ) = 0;
        };

        class ParseBlockFactoryInterface
        {
        public:
            ParseBlockFactoryInterface() : m_fast( true ) {}
            virtual ~ParseBlockFactoryInterface() = 0;

            void setFast( bool onOff ) { m_fast = onOff; }

            virtual std::unique_ptr<ParseBlockInterface> MakeParseBlock() const = 0;

            bool m_fast;
        };

        class SingleThreadedParser
        {
        public:
            SingleThreadedParser( std::istream & input,
                    CatWriterInterface & outputs,
                    ParseBlockFactoryInterface & pbFact );

            ~SingleThreadedParser();

            void parse();
            void setDebug ( bool onOff ) { m_debug = onOff; }

        protected:
            std::istream & m_input;
            CatWriterInterface & m_outputs;
            std::unique_ptr<ParseBlockInterface> m_pb;
            bool m_debug;
        };

        class MultiThreadedParser
        {
        public:
            MultiThreadedParser(
                FILE * input,  // need for speed
                CatWriterInterface & outputs,
                size_t queueLimit,
                size_t threadNum,
                ParseBlockFactoryInterface & pbFact
            );

            void parse();
            size_t num_feed_sleeps = 0;
            static std::atomic< size_t > thread_sleeps;

        private:
            FILE * m_input;
            CatWriterInterface & m_outputs;
            size_t m_queueLimit;
            size_t m_threadNum;
            ParseBlockFactoryInterface & m_pbFact;
        };

    }
}

