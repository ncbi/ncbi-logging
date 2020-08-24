#include "Tool.hpp"
#include "TW_Interface.hpp"

using namespace NCBI::Logging;

std::string tool_version( "1.1.0" );

int main ( int argc, char * argv [], const char * envp []  )
{
    TWParseBlockFactory pbFact;
    Tool t ( tool_version, pbFact );
    return t.run ( argc, argv );
}
