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

static int utf8_utf32 ( uint32_t *dst, const char *begin, const char *end )
{
    int c;
    uint32_t ch;
    const char *src, *stop;

    if ( dst == NULL || begin == NULL || end == NULL )
        return -1;

    if ( begin == end )
        return 0;

    /* non-negative bytes are ASCII-7 */
    c = begin [ 0 ];
    if ( begin [ 0 ] >= 0 )
    {
        dst [ 0 ] = c;
        return 1;
    }
	
    /* the leftmost 24 bits are set
       the rightmost 8 can look like:
       110xxxxx == 2 byte character
       1110xxxx == 3 byte character
       11110xxx == 4 byte character
       111110xx == 5 byte character
       1111110x == 6 byte character
    */
	
    src = begin;
	
    /* invert bits to look at range */
    ch = c;
    c = ~ c;
	
    /* illegal range */
    if ( c >= 0x40 )
        return -1;
	
    /* 2 byte */
    else if ( c >= 0x20 )
    {
        ch &= 0x1F;
        stop = src + 2;
    }
	
    /* 3 byte */
    else if ( c >= 0x10 )
    {
        ch &= 0xF;
        stop = src + 3;
    }
	
    /* 4 byte */
    else if ( c >= 8 )
    {
        ch &= 7;
        stop = src + 4;
    }
	
    /* 5 byte */
    else if ( c >= 4 )
    {
        ch &= 3;
        stop = src + 5;
    }
	
    /* illegal */
    else if ( c < 2 )
        return -1;
    
    /* 6 byte */
    else
    {
        ch &= 1;
        stop = src + 6;
    }
    
    /* must have sufficient input */
    if ( stop > end )
        return 0;
	
    /* complete the character */
    while ( ++ src != stop )
    {
        c = src [ 0 ] & 0x7F;
        if ( src [ 0 ] >= 0 || c >= 0x40 )
            return -1;
        ch = ( ch << 6 ) | c;
    }
	
    /* record the character */
    dst [ 0 ] = ch;
	
    /* return the bytes consumed */
    return ( int ) ( src - begin );
}

static const char * string_rchr ( const char *str, size_t size, uint32_t ch )
{
    int64_t i;

    if ( str == NULL || size == 0 )
        return NULL;

    if ( ch < 128 )
    {
        /* looking for an ASCII character */
        for ( i = ( int64_t ) size - 1; i >= 0; -- i )
        {
            /* perform direct ASCII match */
            if ( str [ i ] == ( char ) ch )
                return ( char * ) & str [ i ];
        }
    }
    else
    {
        int len;
        uint32_t c;
        const char *end;

        for ( i = ( int64_t ) size - 1; i >= 0; -- i )
        {
            /* skip over ASCII */
            for ( ; i >= 0 && str [ i ] > 0; -- i )
                ( void ) 0;
            if ( i < 0 )
                break;

            /* back over UTF-8 */
            for ( end = & str [ i + 1 ]; i >= 0 && ( str [ i ] & 0xC0 ) == 0x80; -- i )
                ( void ) 0;
            if ( i < 0 )
                break;

            /* read UTF-8 */
            len = utf8_utf32 ( & c, & str [ i ], end );
            if ( len <= 0 || & str [ i + len ] != end )
                break;
            if ( c == ch )
                return ( char* ) & str [ i ];
        }
    }
    return NULL;
}

static const char * string_chr ( const char *str, size_t size, uint32_t ch )
{
    size_t i;

    if ( str == NULL || size == 0 )
        return NULL;

    if ( ch < 128 )
    {
        /* looking for an ASCII character */
        for ( i = 0; i < size; ++ i )
        {
            /* perform direct ASCII match */
            if ( str [ i ] == ( char ) ch )
                return ( char * ) & str [ i ];
        }
    }
    else
    {
        int len;
        uint32_t c;
        const char *end = str + size;

        for ( i = 0; i < size; )
        {
            /* skip over ASCII */
            for ( ; i < size && str [ i ] > 0; ++ i )
                ( void ) 0;
            if ( i == size )
                break;

            /* read UTF-8 */
            len = utf8_utf32 ( & c, & str [ i ], end );
            if ( len <= 0 )
                break;
            if ( c == ch )
                return ( char* ) & str [ i ];
            i += len;
        }
    }
    return NULL;
}

void
NCBI::Logging::extract_acc_ext( t_request & r )
{
    const char * qmark = string_chr ( r . path . p, r . path . n, '?' );
    size_t termlen = ( NULL == qmark ) ? r . path . n : ( qmark - r . path . p );
    size_t len_of_leaf = 0;
    const char * last_slash = string_rchr ( r . path . p, termlen, '/' );
    if ( NULL == last_slash )
    {
        // we do not have a slash
        r . accession . p = r . path . p;
        len_of_leaf = termlen;
    }
    else
    {
        // we do have a slash
        r . accession . p = last_slash + 1;
        size_t idx_of_last_slash = ( last_slash - r . path . p );
        len_of_leaf = termlen - idx_of_last_slash - 1;
    }
    // this is common for last_slash NULL or not
    // len_of_leave is the size of accession + extension + 1
    const char * dot = string_chr ( r . accession . p, len_of_leaf , '.' );
    if ( NULL == dot )
    {
        // we do not have a dot
        r . accession . n = len_of_leaf;
        EMPTY_TSTR( r . extension );
    }
    else
    {
        // we do have a dot
        size_t idx_of_first_dot = ( dot - r . accession . p );
        r . accession . n = idx_of_first_dot;
        r . extension . p = r . accession . p + idx_of_first_dot + 1;
        r . extension . n = len_of_leaf - idx_of_first_dot - 1;
    }
}

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
