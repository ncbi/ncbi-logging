#include "Tool.hpp"
#include "GCP_Interface.hpp"

using namespace NCBI::Logging;

std::string tool_version( "1.0.0" );

int main ( int argc, char * argv [], const char * envp []  )
{
    GCPReverseBlockFactory pbFact;
    Tool t ( tool_version, pbFact, ".gcp" );
    return t.run ( argc, argv );
}
