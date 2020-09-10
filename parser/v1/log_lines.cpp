#include "log_lines.hpp"

#include <vector>

#include "op_parser.hpp"
#include "op_scanner.hpp"
#include "aws_parser.hpp"
#include "aws_scanner.hpp"
#include "gcp_parser.hpp"
#include "gcp_scanner.hpp"

#include "types.h"

extern YY_BUFFER_STATE op_scan_reset( const char * input, yyscan_t yyscanner );
extern YY_BUFFER_STATE aws_scan_reset( const char * input, yyscan_t yyscanner );
extern YY_BUFFER_STATE gcp_scan_reset( const char * input, yyscan_t yyscanner );

#ifdef YYDEBUG
    extern int op_debug;
    extern int aws_debug;
    extern int gcp_debug;
#endif

using namespace std;
using namespace NCBI::Logging;

OP_Parser :: OP_Parser( OP_LogLines & p_lines, std::istream & p_input )
: m_lines ( p_lines ), m_input ( p_input )
{
}

void OP_Parser :: setDebug( bool on )
{
    #ifdef YYDEBUG
        op_debug = (int)on;
    #endif
}

void OP_Parser :: setLineFilter( unsigned long int line_nr )
{
    line_filter = line_nr;
}

void OP_Parser :: parse()
{
    string line;
    yyscan_t sc;
    op_lex_init( &sc );
    unsigned long int line_nr = 0;

    op_set_debug( op_debug, sc );

    while( getline( m_input, line ) )
    {
        line_nr++;

        if ( line_filter > 0 )
        {
            if ( line_nr < line_filter )
                continue;
            else if ( line_nr > line_filter )
                break;
        }

        YY_BUFFER_STATE bs = op_scan_reset( line.c_str(), sc );

        if ( op_parse( sc, & m_lines ) != 0 )
        {
            t_str err_line = { line . c_str(), (int)line . size() };
            m_lines . unrecognized( err_line );
        }

        op__delete_buffer( bs, sc );
    }
    op_lex_destroy( sc );
}

/* ============================================================================== */

AWS_Parser :: AWS_Parser( AWS_LogLines & p_lines, std::istream & p_input )
: m_lines ( p_lines ), m_input ( p_input )
{
}

void AWS_Parser :: setDebug( bool on )
{
    #ifdef YYDEBUG
        aws_debug = (int)on;
    #endif
}

void AWS_Parser :: setLineFilter( unsigned long int line_nr )
{
    line_filter = line_nr;
}

void AWS_Parser :: parse()
{
    string line;
    yyscan_t sc;
    unsigned long int line_nr = 0;

    aws_lex_init( &sc );

    aws_set_debug( aws_debug, sc );

    while( getline( m_input, line ) )
    {
        line_nr++;

        if ( line_filter > 0 )
        {
            if ( line_nr < line_filter )
                continue;
            else if ( line_nr > line_filter )
                break;
        }

        YY_BUFFER_STATE bs = aws_scan_reset( line.c_str(), sc );

        if ( aws_parse( sc, & m_lines ) != 0 )
        {
            t_str err_line = { line . c_str(), (int)line . size() };
            m_lines . unrecognized( err_line );
        }

        aws__delete_buffer( bs, sc );
    }
    aws_lex_destroy( sc );
}

/* ============================================================================== */
GCP_Parser :: GCP_Parser( GCP_LogLines & p_lines, std::istream & p_input )
: m_lines ( p_lines ), m_input ( p_input )
{
}

void GCP_Parser :: setDebug( bool on )
{
    #ifdef YYDEBUG
        gcp_debug = (int)on;
    #endif
}

void GCP_Parser :: setLineFilter( unsigned long int line_nr )
{
    line_filter = line_nr;
}

void GCP_Parser :: parse()
{
    string line;
    yyscan_t sc;
    gcp_lex_init( &sc );

    gcp_set_debug( gcp_debug, sc );

    unsigned long int line_nr = 0;
   
    while( getline( m_input, line ) )
    {
        line_nr++;

        if ( line_filter > 0 )
        {
            if ( line_nr < line_filter )
                continue;
            else if ( line_nr > line_filter )
                break;
        }

        YY_BUFFER_STATE bs = gcp_scan_reset( line.c_str(), sc );

        if ( gcp_parse( sc, & m_lines ) != 0 )
        {
            t_str err_line = { line . c_str(), (int)line . size() };
            m_lines . unrecognized( err_line );
        }

        gcp__delete_buffer( bs, sc );
    }
    
    gcp_lex_destroy( sc );
}
