#pragma once

#include <string>

namespace NCBI
{
    namespace Logging
    {
        class ParseBlockFactoryInterface;

        class Tool
        {
        public:
            Tool( const std::string & version, ParseBlockFactoryInterface & pbFact );

            int run ( int argc, char * argv [] );

        private:
            std::string m_version;
            ParseBlockFactoryInterface & m_pbFact;
        };
    }
}
