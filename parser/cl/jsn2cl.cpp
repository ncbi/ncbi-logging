#include "Tool.hpp"
#include "CL_Interface.hpp"

using namespace NCBI::Logging;

std::string tool_version( "1.0.0" );

int main ( int argc, char * argv [], const char * envp []  )
{
    CLReverseBlockFactory pbFact;
    Tool t ( tool_version, pbFact, ".cl" );
    return t.run ( argc, argv );
}
