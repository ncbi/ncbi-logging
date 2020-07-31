#include "AWS_Interface.hpp"

#include <sstream>
#include <iostream>

#include "aws_v2_parser.hpp"
#include "aws_v2_scanner.hpp"
#include "Formatters.hpp"
#include "CatWriters.hpp"

extern YY_BUFFER_STATE aws_scan_reset( const char * input, yyscan_t yyscanner );

using namespace NCBI::Logging;
using namespace std;

LogAWSEvent::LogAWSEvent( FormatterInterface & fmt )
: LogLinesInterface ( fmt )
{
}

void LogAWSEvent::set( AWS_Members m, const t_str & v ) 
{
#define CASE(mem) case mem: m_fmt.addNameValue(#mem, v); break;
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
        m_fmt . addNameValue( "_error", msg );
    }
    catch( const ncbi::JSONUniqueConstraintViolation & e )
    {
        // in case we already inserted a parse-error value, we do nothing...
    }
}

void 
AWSParser::parse()
{ 
    yyscan_t sc;
    aws_lex_init( &sc );

    // set debug output flags
    aws_debug = m_debug ? 1 : 0;            // bison (aws_debug is global)
    aws_set_debug( m_debug ? 1 : 0, sc );   // flex

    FormatterInterface & fmt = m_receiver.GetFormatter();

    unsigned long int line_nr = 0;
    string line;
    while( getline( m_input, line ) )
    {
        line_nr++;

        YY_BUFFER_STATE bs = aws_scan_reset( line.c_str(), sc );

        if ( aws_parse( sc, static_cast<LogAWSEvent*>( & m_receiver ) ) != 0 )
        {
            m_receiver.SetCategory( LogLinesInterface::cat_ugly );
        }

        if ( m_receiver.GetCategory() != LogLinesInterface::cat_good )
        {
            fmt.addNameValue("_line_nr", line_nr);
            t_str t_line;
            t_line.p = line.c_str();
            t_line.n = line.size();
            t_line.escaped = false;
            fmt.addNameValue("_unparsed", t_line);
        }

        //TODO: consider passing the output stream to write() without a temporary string
        stringstream out;
        m_outputs. write ( m_receiver.GetCategory(), fmt . format ( out ).str() );

        aws__delete_buffer( bs, sc );
    }

    aws_lex_destroy( sc );
}

