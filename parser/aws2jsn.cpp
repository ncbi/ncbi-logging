
#include <sstream>
#include <iostream>
#include <iomanip>
#include <string>

#include <cmdline.hpp>

#include "helper.hpp"

#include "AWS_Interface.hpp"
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

static void report( const CatCounter & ctr )
{
    JsonLibFormatter catFmt; 
    catFmt.addNameValue("_total", ctr.get_total());
    catFmt.addNameValue("good", ctr.get_cat_count( LogLinesInterface::cat_good ) );
    catFmt.addNameValue("review", ctr.get_cat_count( LogLinesInterface::cat_review ) );
    catFmt.addNameValue("bad", ctr.get_cat_count( LogLinesInterface::cat_bad ) );
    catFmt.addNameValue("ugly", ctr.get_cat_count( LogLinesInterface::cat_ugly ) );
    stringstream s;
    cerr << catFmt.format(s).str() << endl; 
}

static void handle_aws( const Options & options )
{
    FileCatWriter outputs( options.outputBaseName ); 
    AWSParseBlockFactory pbFact( options . fast );
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
    report( outputs.getCounter() );
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
            handle_aws( options );
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
