#include "LogLinesInterface.hpp"

#include <stdexcept>

using namespace NCBI::Logging;

LogLinesInterface::LogLinesInterface( FormatterInterface & p_fmt ) 
: m_fmt ( p_fmt ), m_cat ( cat_ugly ) 
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
    case ip:        m_fmt.addNameValue("ip",        v); break;
    case referer:   m_fmt.addNameValue("referer",   v); break;
    case unparsed:  m_fmt.addNameValue("unparsed",  v); break;
    
    case agent:
    case request:
    default:
        throw std::logic_error( "invalid LogLinesInterface::Member" ); 
    }
}

void 
LogLinesInterface::setAgent( const t_agent & a )
{
    m_fmt . addNameValue( "agent",                  a . original );
    m_fmt . addNameValue( "vdb_os",                 a . vdb_os );
    m_fmt . addNameValue( "vdb_tool",               a . vdb_tool );
    m_fmt . addNameValue( "vdb_release",            a . vdb_release );
    m_fmt . addNameValue( "vdb_phid_compute_env",   a . vdb_phid_compute_env );
    m_fmt . addNameValue( "vdb_phid_guid",          a . vdb_phid_guid );
    m_fmt . addNameValue( "vdb_phid_session_ip",    a . vdb_phid_session_id );
    m_fmt . addNameValue( "vdb_libc",               a . vdb_libc );
}

void 
LogLinesInterface::setRequest( const t_request & r )
{
    m_fmt . addNameValue( "method",    r . method );
    m_fmt . addNameValue( "path",      r . path );
    m_fmt . addNameValue( "vers",      r . vers );
    m_fmt . addNameValue( "accession", r . accession );
    m_fmt . addNameValue( "filename",  r . filename );
    m_fmt . addNameValue( "extension", r . extension );
}


