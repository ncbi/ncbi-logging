#include "Tool.hpp"
#include "AWS_Interface.hpp"

using namespace NCBI::Logging;

std::string tool_version( "1.0.0" );

int main ( int argc, char * argv [], const char * envp []  )
{
    AWSReverseBlockFactory pbFact;
    Tool t ( tool_version, pbFact, ".aws" );
    return t.run ( argc, argv );
}
