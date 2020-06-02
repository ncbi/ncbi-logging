#include "log_lines.hpp"

#include <sstream>
#include <iomanip>
#include <vector>

#include "op_parser.hpp"
#include "op_scanner.hpp"
#include "aws_parser.hpp"
#include "aws_scanner.hpp"
#include "gcp_parser.hpp"
#include "gcp_scanner.hpp"

#include "types.h"

#ifdef YYDEBUG
    extern int op_debug;
    extern int aws_debug;
    extern int gcp_debug;
#endif

using namespace std;
using namespace NCBI::Logging;

static
string FormatMonth (uint8_t m)
{
    switch (m)
    {
    case 1: return "Jan";
    case 2: return "Feb";
    case 3: return "Mar";
    case 4: return "Apr";
    case 5: return "May";
    case 6: return "Jun";
    case 7: return "Jul";
    case 8: return "Aug";
    case 9: return "Sep";
    case 10: return "Oct";
    case 11: return "Nov";
    case 12: return "Dec";
    default: return "???";
    }
}

string 
NCBI::Logging::t_timepoint::ToString() const
{
    ostringstream out;
    out << setfill ('0'); 

    out << "[";
    out << setw (2) << (int)day << "/";

    out << FormatMonth( month )<<"/";

    out << setw (4) << (int)year <<":";
    out << setw (2) << (int)hour <<":";
    out << setw (2) << (int)minute <<":";
    out << setw (2) << (int)second <<" ";
    out << ( offset < 0 ? "-" : "+" ); 
    out << setw (4) << abs(offset);
    out << "]";

    return out.str();
}

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
void NCBI::Logging::LogGCPHeader::append_fieldname( const t_str &name )
{
    string s_name( ToString( name ) );
    m_fieldnames . push_back( s_name );
}

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
//        gcp_lex_init( &sc ); /* should be out of the loop, but how to reset the state otherwise? */
        // printf( "\nLINE:'%s'\n", line.c_str() );
        YY_BUFFER_STATE bs = gcp__scan_string( line.c_str(), sc );

        res = gcp_parse( sc, & m_lines );
        // return of unrecognized 0 ... coninue, 1 ... abort
        if ( res != 0 )
        {
            t_str err_line = { line . c_str(), (int)line . size() };
            res = m_lines . unrecognized( err_line );
        }

        gcp__delete_buffer( bs, sc );
//        gcp_lex_destroy( sc ); /* should be out of the loop, but how to reset the state otherwise? */
    }
    gcp_lex_destroy( sc );
    return res == 0;
}
