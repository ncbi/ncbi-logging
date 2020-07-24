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

void LogAWSEvent::endLine()  {}

LogLinesInterface::Category 
LogAWSEvent::GetCategory() const 
{ 
    return cat_ugly; 
}

void LogAWSEvent::failedToParse( const t_str & source ) {}
void LogAWSEvent::set( Members m, const t_str & v ) {}
void LogAWSEvent::set( Members m, int64_t v ) {}
void LogAWSEvent::setAgent( const t_agent & a ) {}
void LogAWSEvent::setRequest( const t_request & r ) {}
void LogAWSEvent::setTime( const t_timepoint & t ) {}

ostream & 
LogAWSEvent::format( ostream & out ) const
{
    //TODO iterate over members
    return out;
}

LogLinesInterface::ReportFieldResult 
LogAWSEvent::reportField( Members field, const char * value, const char * message, ReportFieldType type ) 
{
    return proceed;
}

void LogAWSEvent::set( AWS_Members m, const t_str & ) {}

void 
AWSParser::parse()
{ 
    string line;
    yyscan_t sc;
    unsigned long int line_nr = 0;

    aws_lex_init( &sc );

    // set debug output flags
    aws_debug = m_debug ? 1 : 0;            // bison (aws is global)
    aws_set_debug( m_debug ? 1 : 0, sc );   // flex

    while( getline( m_input, line ) )
    {
        line_nr++;

        YY_BUFFER_STATE bs = aws_scan_reset( line.c_str(), sc );

        t_str t_line;

        stringstream out; // consider moving out of the loop
        if ( aws_parse( sc, static_cast<AWS_LogLinesInterface*>( & m_receiver ) ) != 0 )
        {
            FormatterInterface & fmt = m_receiver.GetFormatter();
            fmt.addNameValue("line_nr", line_nr);

            t_line.p = line.c_str();
            t_line.n = line.size();
            t_line.escaped = false;
            fmt.addNameValue("unparsed", t_line);

            m_outputs . write( LogLinesInterface::cat_ugly, fmt.format( out ).str() );
        }
        else 
        {
            //TODO: set line_nr if cmd line option requires            
            m_outputs. write ( m_receiver.GetCategory(), m_receiver . format ( out ).str() );
        }
        m_receiver.endLine();

        aws__delete_buffer( bs, sc );
    }

    aws_lex_destroy( sc );
}

