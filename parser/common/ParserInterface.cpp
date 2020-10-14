#include "ParserInterface.hpp"

#include <thread>

#include "ReceiverInterface.hpp"
#include "Formatters.hpp"
#include "CatWriters.hpp"
#include "Queues.hpp"
#include "LineSplitters.hpp"

using namespace NCBI::Logging;
using namespace std;

ParserDriverInterface::~ParserDriverInterface()
{
}

ParserDriverInterface::ParserDriverInterface( LineSplitterInterface & input,
CatWriterInterface & outputs )
: m_input( input ), m_outputs( outputs )
{
}

ParseBlockInterface :: ~ ParseBlockInterface ()
{
}

void
ParseBlockInterface :: receive_one_line( const char * line, size_t line_size, size_t line_nr )
{
    auto & receiver = GetReceiver();
    receiver . SetCategory( ReceiverInterface::cat_unknown );

    if ( ! format_specific_parse( line, line_size ) )
    {
        receiver . SetCategory( ReceiverInterface::cat_ugly );
    }
    else if ( receiver . GetCategory() == ReceiverInterface::cat_good )
    {
        receiver . SetCategory( receiver.post_process() );
    }

    if ( receiver . GetCategory() != ReceiverInterface::cat_good )
    {
        receiver.GetFormatter().addNameValue("_line_nr", line_nr);
        receiver.setMember( "_unparsed", { line, line_size } );
    }
}

ParseBlockFactoryInterface :: ParseBlockFactoryInterface()
: m_fast( true ), m_nthreads( 1 )
{
}

ParseBlockFactoryInterface :: ~ParseBlockFactoryInterface()
{
}

std::unique_ptr<ParserDriverInterface>
ParseBlockFactoryInterface :: MakeParserDriver(LineSplitterInterface & input, CatWriterInterface & output)
{
    if ( m_nthreads <= 1 )
    {
        return make_unique< SingleThreadedDriver >( input, output, MakeParseBlock() );
    }
    else
    {
        return make_unique< MultiThreadedDriver >( input, output, 100000, m_nthreads, *this );
    }
}

SingleThreadedDriver::SingleThreadedDriver( LineSplitterInterface & input,
        CatWriterInterface & outputs,
        unique_ptr<ParseBlockInterface> pb )
:   ParserDriverInterface( input, outputs ),
    m_debug ( false ),
    m_pb ( pb.release() )
{
}

void
SingleThreadedDriver::parse_all_lines()
{
    m_pb -> SetDebug( m_debug );

    auto & receiver = m_pb -> GetReceiver();
    FormatterInterface & fmt = receiver . GetFormatter();
    unsigned long int line_nr = 0;
    while( m_input.getLine() )
    {
        line_nr++;
        m_pb -> receive_one_line ( m_input.data(), m_input.size(), line_nr );
        m_outputs. write ( receiver . GetCategory(), fmt . format () );
    }
}

MultiThreadedDriver :: MultiThreadedDriver(
    LineSplitterInterface & input,
    CatWriterInterface & outputs,
    size_t queueLimit,
    size_t threadNum,
    ParseBlockFactoryInterface & pbFact )
:   ParserDriverInterface( input, outputs ),
    m_queueLimit ( queueLimit ),
    m_threadNum ( threadNum ),
    m_pbFact ( pbFact )
{
}

// worker-function running multiple times in parallel in the threads...
void parse_thread_worker( unique_ptr<ParseBlockInterface> pb,
             OneWriterManyReadersQueue * in_q,
             OutputQueue * out_q )
{
    auto & receiver = pb -> GetReceiver();
    FormatterInterface & fmt = receiver . GetFormatter();

    while ( true )
    {
        size_t line_nr;
        OneWriterManyReadersQueue::value_type line = in_q -> dequeue( line_nr );
        if ( !line )
        {
            if ( !in_q -> is_open() )
                break;
            else
            {
                const chrono::microseconds WorkerReadWait ( 500 );
                std::this_thread::sleep_for( WorkerReadWait );
            }
        }
        else
        {
            pb -> receive_one_line ( line -> c_str(), line -> size(), line_nr );

            ReceiverInterface::Category cat = receiver . GetCategory();
            shared_ptr< string > output( make_shared< string >( fmt . format() ) );

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
        ReceiverInterface::Category cat;
        shared_ptr< string > line = q -> dequeue( cat );
        if ( !line )
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
            outputs -> write ( cat, *line );
        }
    }
}

void MultiThreadedDriver::parse_all_lines( )
{
    OneWriterManyReadersQueue Q ( m_queueLimit );
    OutputQueue Q_out ( m_queueLimit );

    thread writer_thread( writer, &Q_out, &m_outputs );

    vector<thread> workers;
    for ( auto i = 0; i < m_threadNum; ++i )
    {
        workers.push_back( thread( parse_thread_worker, m_pbFact . MakeParseBlock(), &Q, &Q_out ) );
    }

    try
    {
        while( m_input.getLine() )
        {
            OneWriterManyReadersQueue::value_type s = make_shared< string >( m_input.data(), m_input.size() );
            while ( ! Q.enqueue( s ) )
            {
                this_thread::sleep_for( chrono::microseconds( 100 ) );
            }

        }
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
}
