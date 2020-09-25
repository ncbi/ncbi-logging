#include <gtest/gtest.h>

#include "JWT_Interface.hpp"
#include "Formatters.hpp"

using namespace NCBI::Logging;
using namespace std;

TEST( JWT_Parsing, Create )
{
    std::unique_ptr<FormatterInterface> fmt = make_unique<JsonFastFormatter>();
    JWTReceiver receiver( fmt );
    JWTParseBlock pb( receiver );
}

