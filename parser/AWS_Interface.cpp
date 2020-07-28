#include "AWS_Interface.hpp"

#include <sstream>
#include <iostream>

#include "aws_v2_parser.hpp"
#include "aws_v2_scanner.hpp"
#include "Formatters.hpp"
#include "Classifiers.hpp"

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
}

LogLinesInterface::ReportFieldResult 
LogAWSEvent::reportField( Members field, const char * value, const char * message, ReportFieldType type ) 
{
    return proceed;
}

void 
AWSParser::parse()
{ 
    yyscan_t sc;
    aws_lex_init( &sc );

    // set debug output flags
    aws_debug = m_debug ? 1 : 0;            // bison (aws_debug is global)
    aws_set_debug( m_debug ? 1 : 0, sc );   // flex

    unsigned long int line_nr = 0;
    string line;
    while( getline( m_input, line ) )
    {
        line_nr++;

        YY_BUFFER_STATE bs = aws_scan_reset( line.c_str(), sc );

        stringstream out; // consider moving out of the loop
        if ( aws_parse( sc, static_cast<LogAWSEvent*>( & m_receiver ) ) != 0 )
        {
            FormatterInterface & fmt = m_receiver.GetFormatter();
            fmt.addNameValue("line_nr", line_nr);

            t_str t_line;
            t_line.p = line.c_str();
            t_line.n = line.size();
            t_line.escaped = false;
            fmt.addNameValue("unparsed", t_line);

            m_receiver.SetCategory( LogLinesInterface::cat_ugly );
        }

        m_outputs. write ( m_receiver.GetCategory(), m_receiver . format ( out ).str() );

        aws__delete_buffer( bs, sc );
    }

    aws_lex_destroy( sc );
}

