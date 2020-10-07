#include "ReceiverInterface.hpp"

#include <iomanip>

#include "Formatters.hpp"

using namespace NCBI::Logging;
using namespace std;

ReceiverInterface::ReceiverInterface( FormatterRef p_fmt )
: m_fmt ( p_fmt ), m_cat ( cat_unknown )
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
        if ( m_cat != cat_review )
        {
            stringstream msg;
            msg << ex.what().zmsg << " in '" << mem << "'";
            reportField( msg.str().c_str() );
        }

        // sanitize and retry
        m_fmt -> addNameValue( mem, sanitize( v ) );
    }
}

void
ReceiverInterface::set( Members m, const t_str & v )
{
    switch( m )
    {
    case ip:        setMember( "ip", v ); break;
    case referer:   setMember( "referer", v ); break;
    case unparsed:  setMember( "unparsed", v ); break;
    case agent:     setMember( "agent", v ); break;
    case method:    setMember( "method", v ); break;
    case path:      setMember( "path", v ); break;
    case vers:      setMember( "vers", v ); break;
    }
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

FormatterInterface & ReceiverInterface::GetFormatter()
{
    FormatterInterface * p = m_fmt.get();
    if ( nullptr != p )
        return * p;
    throw std::logic_error( "no formatter available" );
}
