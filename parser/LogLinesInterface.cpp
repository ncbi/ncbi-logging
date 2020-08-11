#include "LogLinesInterface.hpp"

#include <stdexcept>
#include <sstream>
#include <thread>

#include "CatWriters.hpp"
#include "Queues.hpp"

using namespace NCBI::Logging;
using namespace std;

LogLinesInterface::LogLinesInterface( unique_ptr<FormatterInterface> & p_fmt ) 
: m_fmt ( p_fmt.release() ), m_cat ( cat_unknown ) 
{
}

LogLinesInterface::~LogLinesInterface() 
{
}

void 
LogLinesInterface::set( Members m, const t_str & v )
{
    switch( m )
    {
    case ip:        m_fmt -> addNameValue("ip",        v); break;
    case referer:   m_fmt -> addNameValue("referer",   v); break;
    case unparsed:  m_fmt -> addNameValue("unparsed",  v); break;
    
    case agent:
    case request:
    default:
        throw std::logic_error( "invalid LogLinesInterface::Member" ); 
    }
    if ( m_cat == cat_unknown )
        m_cat = cat_good;
}

void 
LogLinesInterface::setAgent( const t_agent & a )
{
    m_fmt -> addNameValue( "agent",                  a . original );
    m_fmt -> addNameValue( "vdb_os",                 a . vdb_os );
    m_fmt -> addNameValue( "vdb_tool",               a . vdb_tool );
    m_fmt -> addNameValue( "vdb_release",            a . vdb_release );
    m_fmt -> addNameValue( "vdb_phid_compute_env",   a . vdb_phid_compute_env );
    m_fmt -> addNameValue( "vdb_phid_guid",          a . vdb_phid_guid );
    m_fmt -> addNameValue( "vdb_phid_session_id",    a . vdb_phid_session_id );
    m_fmt -> addNameValue( "vdb_libc",               a . vdb_libc );
    if ( m_cat == cat_unknown )
        m_cat = cat_good;
}

void 
LogLinesInterface::setRequest( const t_request & r )
{
    m_fmt -> addNameValue( "method",    r . method );
    m_fmt -> addNameValue( "path",      r . path );
    m_fmt -> addNameValue( "vers",      r . vers );
    m_fmt -> addNameValue( "accession", r . accession );
    m_fmt -> addNameValue( "filename",  r . filename );
    m_fmt -> addNameValue( "extension", r . extension );
    if ( m_cat == cat_unknown )
        m_cat = cat_good;
}

ParseBlockInterface :: ~ ParseBlockInterface ()
{
}

ParseBlockFactoryInterface :: ~ParseBlockFactoryInterface()
{
}

SingleThreadedParser::SingleThreadedParser( std::istream & input,  
        CatWriterInterface & outputs,
        ParseBlockFactoryInterface & pbFact )
:   m_input ( input ),
    m_outputs ( outputs ),
    m_pb ( pbFact.MakeParseBlock() ),
    m_debug ( false )
{
}

SingleThreadedParser::~SingleThreadedParser()
{
}

void 
SingleThreadedParser::parse()
{
    auto & receiver = m_pb -> GetReceiver();
    FormatterInterface & fmt = receiver . GetFormatter();

    m_pb -> SetDebug( m_debug );

    unsigned long int line_nr = 0;
    string line;
    while( getline( m_input, line ) )
    {   // TODO: the body of the loop is almost identical to a block in AWSMultiThreadedParser::parser()
        line_nr++;

        receiver . SetCategory( LogLinesInterface::cat_unknown );

        if ( ! m_pb -> Parse( line ) )
        {
            receiver . SetCategory( LogLinesInterface::cat_ugly );
        }
        
        if ( receiver . GetCategory() != LogLinesInterface::cat_good )
        {
            fmt.addNameValue("_line_nr", line_nr);
            fmt.addNameValue("_unparsed", line);
        }

        stringstream out;
        m_outputs. write ( receiver . GetCategory(), fmt . format ( out ).str() );
    }
}

std::atomic< size_t > MultiThreadedParser :: thread_sleeps;

MultiThreadedParser :: MultiThreadedParser( 
    FILE * input,  
    CatWriterInterface & outputs,
    size_t queueLimit,
    size_t threadNum,
    ParseBlockFactoryInterface & pbFact ) 
:   m_input ( input ),
    m_outputs ( outputs ),
    m_queueLimit ( queueLimit ),
    m_threadNum ( threadNum ),
    m_pbFact ( pbFact )
{
    thread_sleeps . store( 0 );
}

void parser( ParseBlockFactoryInterface * factory, 
             OneWriterManyReadersQueue * in_q, 
             OutputQueue * out_q )
{
    auto pb = factory -> MakeParseBlock();
    auto & receiver = pb -> GetReceiver();
    FormatterInterface & fmt = receiver . GetFormatter();

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
                MultiThreadedParser :: thread_sleeps++;
            }
        }
        else
        {   // TODO: this block replicates much of AWSParser::parse()
            receiver . SetCategory( LogLinesInterface::cat_unknown );

            if ( ! pb -> Parse( line ) )
            {
                receiver . SetCategory( LogLinesInterface::cat_ugly );
            }

            if ( receiver . GetCategory() != LogLinesInterface::cat_good )
            {
                fmt.addNameValue("_line_nr", line_nr);
                fmt.addNameValue("_unparsed", line);
            }

            stringstream out;
            LogLinesInterface::Category cat = receiver . GetCategory();
            std::string output = fmt . format( out ).str();

            while ( ! out_q -> enqueue( output, cat ) )
            {
                const chrono::microseconds WorkerWriteWait ( 100 );
                this_thread::sleep_for( WorkerWriteWait );
            }
        }
    }
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

void MultiThreadedParser::parse( )
{
    OneWriterManyReadersQueue Q ( m_queueLimit );
    OutputQueue Q_out ( m_queueLimit );    

    thread writer_thread( writer, &Q_out, &m_outputs );

    vector<thread> workers;
    for ( auto i = 0; i < m_threadNum; ++i )
    {
        workers.push_back( thread( parser, &m_pbFact, &Q, &Q_out ) );
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

