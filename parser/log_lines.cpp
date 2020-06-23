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
NCBI::Logging::ToString( const t_timepoint & t ) 
{
    ostringstream out;
    out << setfill ('0'); 

    out << "[";
    out << setw (2) << (int)t.day << "/";

    out << FormatMonth( t.month )<<"/";

    out << setw (4) << (int)t.year <<":";
    out << setw (2) << (int)t.hour <<":";
    out << setw (2) << (int)t.minute <<":";
    out << setw (2) << (int)t.second <<" ";
    out << ( t.offset < 0 ? "-" : "+" ); 
    out << setw (4) << abs(t.offset);
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

void OP_Parser :: parse()
{
    string line;
    yyscan_t sc;
    op_lex_init( &sc );

    op_set_debug( op_debug, sc );

    while( getline( m_input, line ) )
    {
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

void AWS_Parser :: parse()
{
    string line;
    yyscan_t sc;
    aws_lex_init( &sc );

    aws_set_debug( aws_debug, sc );

    while( getline( m_input, line ) )
    {
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

void GCP_Parser :: parse()
{
    string line;
    yyscan_t sc;
    gcp_lex_init( &sc );

    gcp_set_debug( gcp_debug, sc );

    while( getline( m_input, line ) )
    {
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
