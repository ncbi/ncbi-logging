
#include <sstream>
#include <iostream>
#include <iomanip>
#include <string>

#include <cmdline.hpp>

#include "helper.hpp"

#include "OP_Interface.hpp"
#include "CatWriters.hpp"

using namespace std;
using namespace NCBI::Logging;
using namespace ncbi;

std::string tool_version( "1.1.0" );

struct Options
{
    std::string outputBaseName;
    unsigned int numThreads;
    bool fast;
};

static void handle_op( const Options & options )
{
    FileCatWriter outputs( options.outputBaseName ); 
    OPParseBlockFactory pbFact( options.fast );
    if ( options.numThreads <= 1 )
    {
        SingleThreadedParser p( cin, outputs, pbFact );
        p . parse(); 
    }
    else
    {
        MultiThreadedParser p( stdin, outputs, 100000, options.numThreads, pbFact );
        p . parse();
        std::cout << "num-feed-sleeps = " << p . num_feed_sleeps << endl;
        std::cout << "thread-sleeps = " << MultiThreadedParser :: thread_sleeps . load() << endl;        
    }
    
    JsonLibFormatter f;
    cerr << outputs.getCounter().report( f ) << endl; 
}

int main ( int argc, char * argv [], const char * envp []  )
{
    try
    {
        bool help = false;
        Options options;
        bool vers = false;
        bool fast = false;

        options.numThreads = 1;

        ncbi::Cmdline args( argc, argv );

        args . addOption( vers, "V", "version", "show version" );
        args . addOption( help, "h", "help", "show help" );
        args . addOption( fast, "f", "fast", "use faster json formatting" );
        U32 count;
        args . addOption<unsigned int>( options.numThreads, &count, "t", "threads", "count", "# of parser threads" );

        String outputBaseName;
        args . addParam( outputBaseName, "base-output-name", "base name for the output files" );

        args . parse();
        options.outputBaseName = outputBaseName . toSTLString ();

        if ( help )
            args . help();

        if ( vers )
            cout << "version: " << tool_version << endl;
        
        if ( !help && !vers )
        {
            options . fast = fast;
            handle_op( options );
        }

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