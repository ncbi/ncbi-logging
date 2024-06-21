#include "Tool.hpp"
#include "GCP_Interface.hpp"

using namespace NCBI::Logging;

std::string tool_version( "1.1.1" );

int main ( int argc, char * argv [], const char * envp []  )
{
    GCPParseBlockFactory pbFact;
    Tool t ( tool_version, pbFact, ".jsonl" );
    return t.run ( argc, argv );
}
