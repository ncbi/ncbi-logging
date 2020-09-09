#include "ReceiverInterface.hpp"

#include <stdexcept>
#include <sstream>
#include <thread>

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

ReceiverInterface::ReceiverInterface( unique_ptr<FormatterInterface> & p_fmt )
: m_fmt ( p_fmt.release() ), m_cat ( cat_unknown )
{
}

ReceiverInterface::~ReceiverInterface()
{
}

static
string
sanitize( const t_str & s )
{   // escape non-ASCII characters, including invalid UTF8
    stringstream os;
    for ( auto i = 0; i < s.n; ++i )
    {
        if ( s.p[i] < 0x20 )
        {
            std::ostringstream temp;
            temp << "\\u";
            temp << std::hex << std::setfill('0') << std::setw(4);
            temp << (int)s.p[i];
            os << temp.str();
        }
        else
        {
            os << s.p[i];
        }
    }
    return os.str();
}

void
ReceiverInterface::setMember( const char * mem, const t_str & v )
{
    try
    {
        m_fmt -> addNameValue( mem, v );
    }
    catch ( const ncbi::InvalidUTF8String & ex )
    {
        // report
        stringstream msg;
        msg << ex.what().zmsg << " in '" << mem << "'";
        reportField( msg.str().c_str() );

        // sanitize and retry
        m_fmt -> addNameValue( mem, sanitize( v ) );
    }
    /*
    catch( const ncbi::Exception & ex)
    {
    }
    catch( const std::exception & ex)
    {
    }
    */
}

void
ReceiverInterface::set( Members m, const t_str & v )
{
    switch( m )
    {
    case ip:        setMember( "ip", v); break;
    case referer:   setMember( "referer", v); break;
    case unparsed:  setMember( "unparsed", v); break;

    case agent:
    case request:
    default:
        throw std::logic_error( "invalid ReceiverInterface::Member" );
    }
    if ( m_cat == cat_unknown )
        m_cat = cat_good;
}

void
ReceiverInterface::setAgent( const t_agent & a )
{
    setMember( "agent",                  a . original );
    setMember( "vdb_os",                 a . vdb_os );
    setMember( "vdb_tool",               a . vdb_tool );
    setMember( "vdb_release",            a . vdb_release );
    setMember( "vdb_phid_compute_env",   a . vdb_phid_compute_env );
    setMember( "vdb_phid_guid",          a . vdb_phid_guid );
    setMember( "vdb_phid_session_id",    a . vdb_phid_session_id );
    setMember( "vdb_libc",               a . vdb_libc );
    if ( m_cat == cat_unknown )
        m_cat = cat_good;
}

void
ReceiverInterface::setRequest( const t_request & r )
{
    setMember( "method",    r . method );
    setMember( "path",      r . path );
    setMember( "vers",      r . vers );
    setMember( "accession", r . accession );
    setMember( "filename",  r . filename );
    setMember( "extension", r . extension );
    if ( m_cat == cat_unknown )
        m_cat = cat_good;
}

void ReceiverInterface::reportField( const char * message )
{
    if ( m_cat == cat_unknown || m_cat == cat_good )
        m_cat = cat_review;
    try
    {
        t_str msg { message, strlen( message ), false };
        m_fmt -> addNameValue( "_error", msg );
    }
    catch ( const ncbi::InvalidUTF8String & ex )
    {   // msg has an invalid UTF8 caharacter
        //TODO: m_fmt -> addNameValue( "_error", sanitize(msg) );
    }
    catch( const ncbi::JSONUniqueConstraintViolation & e )
    {
        // in case we already inserted a parse-error value, we do nothing...
    }
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

    if ( receiver . GetCategory() != ReceiverInterface::cat_good )
    {
        receiver.GetFormatter().addNameValue("_line_nr", line_nr);
        receiver.setMember( "_unparsed", { line, line_size, false } );
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
void parser( unique_ptr<ParseBlockInterface> pb,
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
            //TODO: experiment with unique_ptr and/or naked pointers (also see Queues.hpp)
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
        workers.push_back( thread( parser, m_pbFact . MakeParseBlock(), &Q, &Q_out ) );
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
