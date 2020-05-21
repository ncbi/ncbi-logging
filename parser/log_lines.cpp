#include <log_lines.hpp>

#include "log1_parser.hpp"
#include "log1_scanner.hpp"

#ifdef YYDEBUG
    extern int log1_debug;
#endif

using namespace std;
using namespace NCBI::Logging;

LogParser :: LogParser( LogLines & p_lines, std::istream & p_input )
: m_lines ( p_lines ), m_input ( p_input )
{
}

LogParser :: LogParser( LogLines & p_lines )
: m_lines ( p_lines ), m_input ( cin )
{
}

void
LogParser :: setDebug( bool on )
{
    #ifdef YYDEBUG
        log1_debug = (int)on;
    #endif
}

bool
LogParser :: parse()
{
    string line;
    //TODO: loop until EOF
    getline(m_input, line);
    yyscan_t sc;
    log1_lex_init( &sc );

    log1__scan_string( line.c_str(), sc );

    int res = log1_parse( sc, & m_lines );
    log1_lex_destroy( sc );
    return res == 0;
}

