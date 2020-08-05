#include "AWS_Interface.hpp"

#include <sstream>
#include <iostream>
#include <thread>
#include <vector>
#include <stdio.h>

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

std::atomic< size_t > AWSMultiThreadedParser :: thread_sleeps;

AWSMultiThreadedParser :: AWSMultiThreadedParser( 
    std::istream & input,  
    CatWriterInterface & outputs,
    size_t queueLimit,
    size_t threadNum ) 
:   ParserInterface ( input, outputs ),
    m_queueLimit ( queueLimit ),
    m_threadNum ( threadNum )
{
    AWSMultiThreadedParser :: thread_sleeps . store( 0 );
}

void parser( AWSReceiverFactory * factory, 
             OneWriterManyReadersQueue * in_q, 
             OutputQueue * out_q )
{
    const unsigned int WorkerWait = 500;
    auto receiver = factory -> MakeReceiver();
    FormatterInterface & fmt = receiver -> GetFormatter();
    yyscan_t sc;
    aws_lex_init( &sc );

    while ( true )
    {
        string line;
        size_t line_nr;
        if ( !in_q -> dequeue( line, line_nr ) )
        {
            if ( !in_q -> is_open() )
                return;
            else
            {
                std::this_thread::sleep_for( std::chrono::microseconds( WorkerWait ) );
                AWSMultiThreadedParser :: thread_sleeps++;
            }
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
            LogLinesInterface::Category cat = receiver -> GetCategory();
            std::string output = fmt . format( out ).str();

            while ( ! out_q -> enqueue( output, cat ) ) //TODO: pass in line_nr
            {
                this_thread::sleep_for( chrono::microseconds( 100 ) );
            }

            aws__delete_buffer( bs, sc );
        }
    }
    
    aws_lex_destroy( sc );
}

void writer( OutputQueue * q,
             CatWriterInterface * outputs )
{
    const unsigned int WriterWait = 500;

    while ( true )
    {
        string line;
        LogLinesInterface::Category cat;
        if ( !q -> dequeue( line, cat ) )
        {
            if ( !q -> is_open() )
                return;
            else
                std::this_thread::sleep_for( std::chrono::microseconds( WriterWait ) );
        }
        else
        {
            outputs -> write ( cat, line );
        }
    }
}

void AWSMultiThreadedParser::parse( )
{
    OneWriterManyReadersQueue Q ( m_queueLimit );
    OutputQueue Q_out ( m_queueLimit );    

    thread writer_thread( writer, &Q_out, &m_outputs );

    vector<thread> workers;
    for ( auto i = 0; i < m_threadNum; ++i )
    {
        workers.push_back( thread( parser, &m_factory, &Q, &Q_out ) );
    }

    FILE * ifile = stdin;
    size_t linesz = 0;
    char * line = nullptr;
    while( getline( &line, &linesz, ifile ) > 0 )
    {
        std::string s( line, linesz );
        while ( ! Q.enqueue( s ) ) //TODO: pass in line_nr
        {
            this_thread::sleep_for( chrono::microseconds( 100 ) );
            num_feed_sleeps++;
        }

    }
    free( line );

    Q.close();
    for ( auto i = 0; i < m_threadNum; ++i )
    {
       workers[i].join();
    }

    Q_out.close();
    writer_thread.join();

    std::cout << "q.max = " << Q.m_max << std::endl;
}

