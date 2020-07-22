#include "AWSToJsonImpl.hpp"

#include <sstream>
#include <iostream>
#include <iomanip>
#include <string>

#include <cmdline.hpp>

#include "helper.hpp"

using namespace std;
using namespace NCBI::Logging;
using namespace ncbi;

static void handle_aws( const Options & options )
{
    AWSToJsonLogLines event_receiver( cout, options );
    AWSParser p( event_receiver, cin );
    p . setDebug( options . parser_debug );
    p . setLineFilter( options . selected_line );
    p . parse(); // does the parsing and generates the report
}

int main ( int argc, char * argv [], const char * envp []  )
{
    try
    {
        ncbi::String format( "op" );
        bool help = false;
        Options options;
        bool vers = false;
        bool no_report = false;

        ncbi::Cmdline args( argc, argv );
        args . addOption( options . parser_debug, "d", "debug", "run with parser-debug-output" );
        args . addOption( options . print_line_nr, "p", "print-line-nr", "print line numbers" );
        args . addOption( options . path_only, "a", "path-only", "print only the path found ( not json )" );
        args . addOption( options . agent_only, "g", "agent-only", "print only the agent found ( not json )" );

        args . addOption( options . selected_line, nullptr, "l", "line-to-select", "line-nr", "select only this line from input (1-based)" );

        args . addOption( vers, "V", "version", "show version" );
        args . addOption( help, "h", "help", "show help" );

        args . parse();

        options.report = ! no_report;

        if ( help )
            args . help();

        if ( vers )
            cout << "version: 1.1" << endl;
        
        if ( !help && !vers )
            handle_aws( options );

        return 0;
    }
    catch( const exception & e)
    {
        cerr << "Exception caught: " << e.what() << endl;
        return 1;
    }
    catch( const ncbi::InvalidArgument & e)
    {
        cerr << "Invalid Argument: " << e.what() << endl;
        return 1;
    }
    catch(...)
    {
        cerr << "Unknown exception caught" << endl;
        return 2;
    }
    return 0;
}
