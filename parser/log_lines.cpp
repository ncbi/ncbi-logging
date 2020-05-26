#include <log_lines.hpp>

#include "op_parser.hpp"
#include "op_scanner.hpp"
#include "aws_parser.hpp"
#include "aws_scanner.hpp"
#include "gcp_parser.hpp"
#include "gcp_scanner.hpp"

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

OP_Parser :: OP_Parser( OP_LogLines & p_lines )
: m_lines ( p_lines ), m_input ( cin )
{
}

void OP_Parser :: setDebug( bool on )
{
    #ifdef YYDEBUG
        op_debug = (int)on;
    #endif
}

bool OP_Parser :: parse()
{
    int res = 0;
    string line;
    yyscan_t sc;
    op_lex_init( &sc );

    while( 0 == res && getline( m_input, line ) )
    {
        op__scan_string( line.c_str(), sc );

        res = op_parse( sc, & m_lines );
        // return of unrecognized 0 ... coninue, 1 ... abort
        if ( res != 0 )
        {
            t_str err_line = { line . c_str(), (int)line . size() };
            res = m_lines . unrecognized( err_line );
        }
    }
    op_lex_destroy( sc );
    return res == 0;
}

/* ============================================================================== */

AWS_Parser :: AWS_Parser( AWS_LogLines & p_lines, std::istream & p_input )
: m_lines ( p_lines ), m_input ( p_input )
{
}

AWS_Parser :: AWS_Parser( AWS_LogLines & p_lines )
: m_lines ( p_lines ), m_input ( cin )
{
}

void AWS_Parser :: setDebug( bool on )
{
    #ifdef YYDEBUG
        aws_debug = (int)on;
    #endif
}

bool AWS_Parser :: parse()
{
    int res = 0;
    string line;
    yyscan_t sc;
    aws_lex_init( &sc );

    while( 0 == res && getline( m_input, line ) )
    {
        aws__scan_string( line.c_str(), sc );

        res = aws_parse( sc, & m_lines );
        // return of unrecognized 0 ... coninue, 1 ... abort
        if ( res != 0 )
        {
            t_str err_line = { line . c_str(), (int)line . size() };
            res = m_lines . unrecognized( err_line );
        }
    }
    aws_lex_destroy( sc );
    return res == 0;
}

/* ============================================================================== */

GCP_Parser :: GCP_Parser( GCP_LogLines & p_lines, std::istream & p_input )
: m_lines ( p_lines ), m_input ( p_input )
{
}

GCP_Parser :: GCP_Parser( GCP_LogLines & p_lines )
: m_lines ( p_lines ), m_input ( cin )
{
}

void GCP_Parser :: setDebug( bool on )
{
    #ifdef YYDEBUG
        gcp_debug = (int)on;
    #endif
}

bool GCP_Parser :: parse()
{
    int res = 0;
    string line;
    yyscan_t sc;
    gcp_lex_init( &sc );

    while( 0 == res && getline( m_input, line ) )
    {
        gcp__scan_string( line.c_str(), sc );

        res = gcp_parse( sc, & m_lines );
        // return of unrecognized 0 ... coninue, 1 ... abort
        if ( res != 0 )
        {
            t_str err_line = { line . c_str(), (int)line . size() };
            res = m_lines . unrecognized( err_line );
        }
    }
    gcp_lex_destroy( sc );
    return res == 0;
}
