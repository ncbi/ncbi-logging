#pragma once

#include <string>
#include <cstring>

#include "types.h"

namespace NCBI
{
    namespace Logging
    {
        class FormatterInterface;
        class LogLinesInterface;
        class ClassificationInterface;

        typedef enum { acc_before = 0, acc_inside, acc_after } eAccessionMode;

        typedef struct t_request
        {
            t_str method;
            t_str path;
            t_str vers;
            t_str accession;
            t_str filename;
            t_str extension;

            t_str server; // only used in OP

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

        struct CommonLogEvent
        {
            t_str       ip;
            t_str       referer;
            t_agent     agent;
            t_str       unparsed;
            t_request   request;

            CommonLogEvent()
            {
                EMPTY_TSTR( ip );
                EMPTY_TSTR( referer );
                InitAgent( agent );
                EMPTY_TSTR( unparsed );
                InitRequest( request );
            }
        };

        struct LogLinesInterface
        {
            LogLinesInterface( FormatterInterface & p_fmt ) 
            : m_fmt ( p_fmt ), m_cat ( cat_ugly ) 
            {
            }

            virtual ~LogLinesInterface() {}

            virtual void endLine() = 0;

            typedef enum { cat_review, cat_good, cat_bad, cat_ugly } Category;

            virtual Category GetCategory() const { return m_cat; }

            // LogLinesInterface();
            // virtual LogLinesInterface();

            virtual void failedToParse( const t_str & source ) = 0;

            typedef enum { 
                ip,
                referer,
                unparsed,
                LastMemberId = unparsed
            } Members;

            virtual void set( Members m, const t_str & v ) = 0;
            virtual void set( Members m, int64_t v ) = 0;
            virtual void setAgent( const t_agent & a ) = 0;
            virtual void setRequest( const t_request & r ) = 0;

            // iterate over members with fmt.addNameValue(), call fmt.format(out) at the end
            virtual std::stringstream & format( std::stringstream & out ) const = 0; 

            // could be a throw instead of abort, but we need to make sure the parser terminates cleanly if we throw.
            typedef enum { proceed, abort } ReportFieldResult;
            typedef enum { suspect, bad } ReportFieldType;
            virtual ReportFieldResult reportField( Members field, const char * value, const char * message, ReportFieldType type ) = 0;

            FormatterInterface & GetFormatter() { return m_fmt; }

        protected:
            FormatterInterface & m_fmt;
            Category m_cat;
        };

        class ParserInterface 
        {
        public:
            ParserInterface( std::istream & input,  
                             LogLinesInterface & receiver,
                             ClassificationInterface & outputs ) 
            :   m_input ( input ),
                m_receiver ( receiver ), 
                m_outputs ( outputs ),
                m_debug ( false )
            {
            }

            virtual ~ParserInterface() {};

            virtual void parse() = 0;

            void setDebug ( bool onOff ) { m_debug = onOff; }

        protected:
            std::istream & m_input;
            LogLinesInterface & m_receiver; // we use its built-in formatter
            ClassificationInterface & m_outputs;
            bool m_debug;
        };

    }
}

