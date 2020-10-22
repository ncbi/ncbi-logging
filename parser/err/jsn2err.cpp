#include "Tool.hpp"
#include "ERR_Interface.hpp"

using namespace NCBI::Logging;

std::string tool_version( "0.0.1" );

int main ( int argc, char * argv [], const char * envp []  )
{
    ERRReverseBlockFactory pbFact;
    Tool t ( tool_version, pbFact, ".err" );
    return t.run ( argc, argv );
}
