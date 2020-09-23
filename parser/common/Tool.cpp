#include "Tool.hpp"

#include <cmdline.hpp>

#include "ParserInterface.hpp"
#include "CatWriters.hpp"
#include "LineSplitters.hpp"
#include "Formatters.hpp"

using namespace std;
using namespace NCBI::Logging;
using namespace ncbi;

Tool::Tool( const string & version, ParseBlockFactoryInterface & pbFact, const std::string &extension )
:   m_version ( version ),
    m_pbFact( pbFact ),
    m_extension( extension )
{
}

int
Tool::run ( int argc, char * argv [] )
{
    bool help = false;
    bool vers = false;
    ncbi::Cmdline args( argc, argv );
    try
    {
        args . addOption( vers, "V", "version", "show version" );
        args . addOption( help, "h", "help", "show help" );

        bool fast = false;
        args . addOption( fast, "f", "fast", "use faster json formatting" );

        unsigned int numThreads = 1;
        U32 count;
        args . addOption<unsigned int>( numThreads, &count, "t", "threads", "count", "# of parser threads" );

        String outputBaseName;
        args . addParam( outputBaseName, "base-output-name", "base name for the output files" );

        args . parse();

        if ( help )
            args . help();

        if ( vers )
            cout << "version: " << m_version << endl;

        if ( !help && !vers )
        {
            CLineSplitter input( stdin );
            FileCatWriter outputs( outputBaseName . toSTLString (), m_extension );
            m_pbFact.setFast( fast );
            m_pbFact.setNumThreads( numThreads );
            m_pbFact.MakeParserDriver( input, outputs ) -> parse_all_lines();

            std::ofstream report( outputBaseName . toSTLString () + ".stats" + m_extension );
            JsonLibFormatter f;
            report << outputs.getCounter().report( f ) << endl;
        }

        return 0;
    }
    catch( const exception & e )
    {
        cerr << "Exception caught: " << e.what() << endl;
        return 1;
    }
    catch( const ncbi::InvalidArgument & e )
    {
        if ( help || vers )
        {
            if ( help )
                args . help();
            if ( vers )
                cout << "version: " << m_version << endl;
        }
        else
        {
            cerr << "NCBI Exception caught: " << e.what() << endl;
            return 1;
        }
    }
    catch( const ncbi::Exception & e )
    {
        cerr << "NCBI Exception caught: " << e.what() << endl;
        return 1;
    }
    catch( ... )
    {
        cerr << "Unknown exception caught" << endl;
        return 2;
    }
    return 0;
}
