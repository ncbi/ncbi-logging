#include "AWS_Interface.hpp"

#include <sstream>
#include <iostream>
#include <thread>
#include <vector>

#include "aws_v2_parser.hpp"
#include "aws_v2_scanner.hpp"
#include "Formatters.hpp"
#include "CatWriters.hpp"
#include "helper.hpp"

extern YY_BUFFER_STATE aws_scan_reset( const char * input, yyscan_t yyscanner );

using namespace NCBI::Logging;
using namespace std;

LogAWSEvent::LogAWSEvent( unique_ptr<FormatterInterface> & fmt )
: LogLinesInterface ( fmt )
{
}

void LogAWSEvent::set( AWS_Members m, const t_str & v ) 
{
#define CASE(mem) case mem: m_fmt -> addNameValue(#mem, v); break;
    switch( m )
    {
    CASE( owner )
    CASE( bucket )
    CASE( time )
    CASE( requester )
    CASE( request_id )
    CASE( operation )
    CASE( key )
    CASE( res_code )
    CASE( error )
    CASE( res_len )
    CASE( obj_size )
    CASE( total_time )
    CASE( turnaround_time )
    CASE( version_id )
    CASE( host_id )
    CASE( sig_ver )
    CASE( cipher_suite )
    CASE( auth_type )
    CASE( host_header )
    CASE( tls_version )
    default: LogLinesInterface::set((LogLinesInterface::Members)m, v); 
    }
#undef CASE
    if ( m_cat == cat_unknown )
        m_cat = cat_good;
}

void LogAWSEvent::reportField( const char * message ) 
{
    if ( m_cat == cat_unknown || m_cat == cat_good )
        m_cat = cat_review;
    try
    {
        t_str msg { message, (int)strlen( message ), false };
        m_fmt -> addNameValue( "_error", msg );
    }
    catch( const ncbi::JSONUniqueConstraintViolation & e )
    {
        // in case we already inserted a parse-error value, we do nothing...
    }
}

AWSParser::AWSParser( 
    std::istream & input,  
    CatWriterInterface & outputs ) 
:   ParserInterface ( input, outputs )
{
}

void 
AWSParser::parse()
{ 
    yyscan_t sc;
    aws_lex_init( &sc );

    // set debug output flags
    aws_debug = m_debug ? 1 : 0;            // bison (aws_debug is global)
    aws_set_debug( m_debug ? 1 : 0, sc );   // flex

    auto m_receiver = m_factory.MakeReceiver();
    FormatterInterface & fmt = m_receiver -> GetFormatter();

    unsigned long int line_nr = 0;
    string line;
    while( getline( m_input, line ) )
    {   // TODO: the body of the loop is almost identical to a block in AWSMultiThreadedParser::parser()
        line_nr++;

        YY_BUFFER_STATE bs = aws_scan_reset( line.c_str(), sc );

        m_receiver -> SetCategory( LogLinesInterface::cat_unknown );

        if ( aws_parse( sc, static_cast<LogAWSEvent*>( m_receiver.get() ) ) != 0 )
        {
            m_receiver -> SetCategory( LogLinesInterface::cat_ugly );
        }
        
        if ( m_receiver -> GetCategory() != LogLinesInterface::cat_good )
        {
            fmt.addNameValue("_line_nr", line_nr);
            fmt.addNameValue("_unparsed", line);
        }

        //TODO: consider passing the output stream to write() without a temporary string
        stringstream out;
        m_outputs. write ( m_receiver -> GetCategory(), fmt . format ( out ).str() );

        aws__delete_buffer( bs, sc );
    }

    aws_lex_destroy( sc );
}

AWSMultiThreadedParser :: AWSMultiThreadedParser( 
    std::istream & input,  
    CatWriterInterface & outputs,
    size_t queueLimit,
    size_t threadNum ) 
:   ParserInterface ( input, outputs ),
    m_queueLimit ( queueLimit ),
    m_threadNum ( threadNum )
{
}

void parser( AWSReceiverFactory * factory, 
             OneWriterManyReadersQueue * q, 
             CatWriterInterface * outputs )
{
    auto receiver = factory -> MakeReceiver();
    FormatterInterface & fmt = receiver -> GetFormatter();
    
    const unsigned int WorkerWait = 10;
    yyscan_t sc;
    aws_lex_init( &sc );

    while ( true )
    {
        string line;
        size_t line_nr;
        if ( !q -> dequeue( line, line_nr ) )
        {
            if ( !q -> is_open() )
                return;
            else
                std::this_thread::sleep_for( std::chrono::milliseconds( WorkerWait ) );
        }
        else
        {   // TODO: this block is almost identical to the body of the loop in AWSMultiThreadedParser::parser
            YY_BUFFER_STATE bs = aws_scan_reset( line.c_str(), sc );

            receiver -> SetCategory( LogLinesInterface::cat_unknown );

            if ( aws_parse( sc, static_cast<LogAWSEvent*>( receiver.get() ) ) != 0 )
            {
                receiver -> SetCategory( LogLinesInterface::cat_ugly );
            }

            if ( receiver -> GetCategory() != LogLinesInterface::cat_good )
            {
                fmt.addNameValue("_line_nr", line_nr);
                fmt.addNameValue("_unparsed", line);
            }

            //TODO: consider passing the output stream to write() without a temporary string
            stringstream out;
            outputs -> write ( receiver -> GetCategory(), fmt . format ( out ).str() );

            aws__delete_buffer( bs, sc );
        }
    }

    aws_lex_destroy( sc );
}

void
AWSMultiThreadedParser::parse( )
{
    OneWriterManyReadersQueue Q ( m_queueLimit );
    vector<thread> workers;
    for ( auto i = 0; i < m_threadNum; ++i )
    {
        workers.push_back( thread( parser, &m_factory, &Q, &m_outputs ) );
    }

    string line;
    while( getline( m_input, line ) )
    {
        while ( ! Q.enqueue( line ) ) //TODO: pass in line_nr
        {
            this_thread::sleep_for(chrono::milliseconds(1));
        }
    }

    Q.close();
    for ( auto i = 0; i < m_threadNum; ++i )
    {
        workers[i].join();
    }
}

