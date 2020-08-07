#include "OP_Interface.hpp"

#include <sstream>
#include <iostream>
#include <thread>
#include <vector>
#include <stdio.h>

#include "op_v2_parser.hpp"
#include "op_v2_scanner.hpp"
#include "Formatters.hpp"
#include "CatWriters.hpp"
#include "Queues.hpp"

extern YY_BUFFER_STATE op_scan_reset( const char * input, yyscan_t yyscanner );

using namespace NCBI::Logging;
using namespace std;

LogOPEvent::LogOPEvent( unique_ptr<FormatterInterface> & fmt )
: LogLinesInterface ( fmt )
{
}

void LogOPEvent::set( OP_Members m, const t_str & v ) 
{
#define CASE(mem) case mem: m_fmt -> addNameValue(#mem, v); break;
    switch( m )
    {
    CASE( owner )
    CASE( time )
    CASE( server )
    CASE( user )
    CASE( req_time )
    CASE( forwarded )
    CASE( port )
    CASE( req_len )
    CASE( res_code )
    CASE( res_len )
    default: LogLinesInterface::set((LogLinesInterface::Members)m, v); 
    }
#undef CASE
    if ( m_cat == cat_unknown )
        m_cat = cat_good;
}

void LogOPEvent::reportField( const char * message ) 
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

OPParser::OPParser( 
    std::istream & input,  
    CatWriterInterface & outputs ) 
:   ParserInterface ( outputs ), m_input ( input )
{
}

void 
OPParser::parse()
{ 
    yyscan_t sc;
    op_lex_init( &sc );

    // set debug output flags
    op_debug = m_debug ? 1 : 0;            // bison (op_debug is global)
    op_set_debug( m_debug ? 1 : 0, sc );   // flex

    auto m_receiver = m_factory.MakeReceiver();
    FormatterInterface & fmt = m_receiver -> GetFormatter();

    unsigned long int line_nr = 0;
    string line;
    while( getline( m_input, line ) )
    {   // TODO: the body of the loop is almost identical to a block in OPMultiThreadedParser::parser()
        line_nr++;

        YY_BUFFER_STATE bs = op_scan_reset( line.c_str(), sc );

        m_receiver -> SetCategory( LogLinesInterface::cat_unknown );

        if ( op_parse( sc, static_cast<LogOPEvent*>( m_receiver.get() ) ) != 0 )
        {
            m_receiver -> SetCategory( LogLinesInterface::cat_ugly );
        }
        
        if ( m_receiver -> GetCategory() != LogLinesInterface::cat_good )
        {
            fmt.addNameValue("_line_nr", line_nr);
            fmt.addNameValue("_unparsed", line);
        }

        stringstream out;
        m_outputs. write ( m_receiver -> GetCategory(), fmt . format ( out ).str() );

        op__delete_buffer( bs, sc );
    }

    op_lex_destroy( sc );
}

std::atomic< size_t > OPMultiThreadedParser :: thread_sleeps;

OPMultiThreadedParser :: OPMultiThreadedParser( 
    FILE * input,  
    CatWriterInterface & outputs,
    size_t queueLimit,
    size_t threadNum ) 
:   ParserInterface ( outputs ),
    m_input ( input ),
    m_queueLimit ( queueLimit ),
    m_threadNum ( threadNum )
{
    OPMultiThreadedParser :: thread_sleeps . store( 0 );
}

void parser( OPReceiverFactory * factory, 
             OneWriterManyReadersQueue * in_q, 
             OutputQueue * out_q )
{
    auto receiver = factory -> MakeReceiver();
    FormatterInterface & fmt = receiver -> GetFormatter();
    yyscan_t sc;
    op_lex_init( &sc );

    while ( true )
    {
        string line;
        size_t line_nr;
        if ( !in_q -> dequeue( line, line_nr ) )
        {
            if ( !in_q -> is_open() )
                break;
            else
            {
                const chrono::microseconds WorkerReadWait ( 500 );
                std::this_thread::sleep_for( WorkerReadWait );
                OPMultiThreadedParser :: thread_sleeps++;
            }
        }
        else
        {   // TODO: this block replicates much of OPParser::parse()
            YY_BUFFER_STATE bs = op_scan_reset( line.c_str(), sc );

            receiver -> SetCategory( LogLinesInterface::cat_unknown );

            if ( op_parse( sc, static_cast<LogOPEvent*>( receiver.get() ) ) != 0 )
            {
                receiver -> SetCategory( LogLinesInterface::cat_ugly );
            }

            if ( receiver -> GetCategory() != LogLinesInterface::cat_good )
            {
                fmt.addNameValue("_line_nr", line_nr);
                fmt.addNameValue("_unparsed", line);
            }

            stringstream out;
            LogLinesInterface::Category cat = receiver -> GetCategory();
            std::string output = fmt . format( out ).str();

            while ( ! out_q -> enqueue( output, cat ) )
            {
                const chrono::microseconds WorkerWriteWait ( 100 );
                this_thread::sleep_for( WorkerWriteWait );
            }

            op__delete_buffer( bs, sc );
        }
    }
    
    op_lex_destroy( sc );
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
            {
                std::this_thread::sleep_for( std::chrono::microseconds( WriterWait ) );
            }
        }
        else
        {
            outputs -> write ( cat, line );
        }
    }
}

void OPMultiThreadedParser::parse( )
{
    OneWriterManyReadersQueue Q ( m_queueLimit );
    OutputQueue Q_out ( m_queueLimit );    

    thread writer_thread( writer, &Q_out, &m_outputs );

    vector<thread> workers;
    for ( auto i = 0; i < m_threadNum; ++i )
    {
        workers.push_back( thread( parser, &m_factory, &Q, &Q_out ) );
    }

    try
    {
        size_t allocsz = 0;
        ssize_t linesz = 0;
        char * line = nullptr;
        while( ( linesz = getline( &line, &allocsz, m_input ) ) > 0 )
        {   
            while ( ! Q.enqueue( string( line, linesz ) ) ) //TODO: pass in line_nr
            {
                this_thread::sleep_for( chrono::microseconds( 100 ) );
                num_feed_sleeps++;
            }

        }
        free( line );
    }
    catch(...)
    {
        Q.close();
        for ( auto i = 0; i < m_threadNum; ++i )
        {
            workers[i].join();
        }

        Q_out.close();
        writer_thread.join();
        throw;
    }

    Q.close();
    for ( auto i = 0; i < m_threadNum; ++i )
    {
       workers[i].join();
    }

    Q_out.close();
    writer_thread.join();

    //std::cout << "q.max = " << Q.m_max << std::endl;
}

