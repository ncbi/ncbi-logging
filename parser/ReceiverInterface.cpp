#include "ReceiverInterface.hpp"

#include <stdexcept>
#include <sstream>
#include <thread>

#include "CatWriters.hpp"
#include "Queues.hpp"

using namespace NCBI::Logging;
using namespace std;

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
    catch( const ncbi::Exception & ex)
    {
cout<<"ncbi::Exception "<<ex.what()<<endl;
    }
    catch( const std::exception & ex)
    {
cout<<"std::exception "<<ex.what()<<endl;
    }
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

        receiver . SetCategory( ReceiverInterface::cat_unknown );

        if ( ! m_pb -> Parse( line ) )
        {
            receiver . SetCategory( ReceiverInterface::cat_ugly );
        }

        if ( receiver . GetCategory() != ReceiverInterface::cat_good )
        {
            fmt.addNameValue("_line_nr", line_nr);
            receiver.setMember( "_unparsed", { line.c_str(), line.size(), false } );
        }

        m_outputs. write ( receiver . GetCategory(), fmt . format () );
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

ssize_t readline(FILE *stream, char *buf, size_t size)
{
  if (size == 0)
    return 0;

  size_t count;
  int c = 0;
  for (count = 0; c != '\n' && count < size - 1; count++)
  {
    c = getc(stream);

    if (c == EOF) {
      if (count == 0)
        return -1;
      break;
    }

    buf[count] = (char) c;
  }

  buf[count] = '\0';
  return (ssize_t) count;
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
                MultiThreadedParser :: thread_sleeps++;
            }
        }
        else
        {   // TODO: this block replicates much of AWSParser::parse()
            receiver . SetCategory( ReceiverInterface::cat_unknown );

            if ( ! pb -> Parse( *line ) )
            {
                receiver . SetCategory( ReceiverInterface::cat_ugly );
            }

            if ( receiver . GetCategory() != ReceiverInterface::cat_good )
            {
                fmt . addNameValue( "_line_nr", line_nr );
                receiver . setMember( "_unparsed", { line -> c_str(), line -> size(), false } );
            }

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
        char line [4096];
        ssize_t linesz = 0;
        while( ( linesz = readline( m_input, line, sizeof(line) ) ) > 0 )
        {
            if ( line[ linesz - 1 ] == 0x0A ) linesz--;

            OneWriterManyReadersQueue::value_type s = make_shared< string >( line, linesz );
            while ( ! Q.enqueue( s ) )
            {
                this_thread::sleep_for( chrono::microseconds( 100 ) );
                num_feed_sleeps++;
            }

        }
        //free( line );
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

