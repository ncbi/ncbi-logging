
#include <sstream>
#include <iostream>
#include <iomanip>
#include <string>

#include <cmdline.hpp>

#include "helper.hpp"

#include "AWS_Interface.hpp"
#include "Classifiers.hpp"

using namespace std;
using namespace NCBI::Logging;
using namespace ncbi;

std::string tool_version( "1.1.0" );

struct Options
{
    std::string outputBaseName;
};

static void handle_aws( const Options & options )
{
    JsonLibFormatter jsonFmt; 
    LogAWSEvent receiver( jsonFmt );
    FileClassifier outputs( options.outputBaseName ); 
    AWSParser p( cin, receiver, outputs );
    p . parse(); // does the parsing and generates the report
}

int main ( int argc, char * argv [], const char * envp []  )
{
    try
    {
        bool help = false;
        Options options;
        bool vers = false;

        ncbi::Cmdline args( argc, argv );

        args . addOption( vers, "V", "version", "show version" );
        args . addOption( help, "h", "help", "show help" );

        String outputBaseName;
        args . addParam( outputBaseName, "base-output-name", "base name for the output files" );

        args . parse();
        options.outputBaseName = outputBaseName . toSTLString ();

        if ( help )
            args . help();

        if ( vers )
            cout << "version: " << tool_version << endl;
        
        if ( !help && !vers )
            handle_aws( options );

        return 0;
    }
    catch( const exception & e )
    {
        cerr << "Exception caught: " << e.what() << endl;
        return 1;
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
