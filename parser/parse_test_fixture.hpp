#pragma once

#include <gtest/gtest.h>

#include <sstream>
#include "CatWriters.hpp"
#include "ReceiverInterface.hpp"

namespace NCBI
{
    namespace Logging
    {

        class TestCatWriter : public StringCatWriter
        {   // capture "ignored"
        public:
            virtual void write( ReceiverInterface::Category cat, const std::string & s )
            {
                if ( cat == ReceiverInterface::cat_ignored )
                {
                    ignored << s << std::endl;
                }
                else
                {
                    return StringCatWriter::write(cat, s);
                }
                
            }

            const std::string get_ignored()   { return ignored . str(); }

            std::stringstream ignored;
        };

        template< typename ParseBlockFactory >
        class ParseTestFixture : public ::testing::Test
        {
            public :
                void try_to_parse( std::string line, bool debug = false )
                {
                    std::istringstream ss( line );
                    ParseBlockFactory pbFact;
                    SingleThreadedParser p( ss, s_outputs, pbFact );
                    p . setDebug( debug );
                    p . parse(); // does the parsing and generates the report
                }

                std::string try_to_parse_good( std::string line, bool debug = false )
                {
                    try_to_parse( line, debug );
                    return s_outputs.get_good();
                }

                std::string try_to_parse_bad( std::string line, bool debug = false )
                {
                    try_to_parse( line, debug );
                    return s_outputs.get_bad();
                }

                std::string try_to_parse_ugly( std::string line, bool debug = false )
                {
                    try_to_parse( line, debug );
                    return s_outputs.get_ugly();
                }

                std::string try_to_parse_review( std::string line, bool debug = false )
                {
                    try_to_parse( line, debug );
                    return s_outputs.get_review();
                }

                std::string try_to_parse_ignored( std::string line, bool debug = false )
                {
                    try_to_parse( line, debug );
                    return s_outputs.get_ignored();
                }

                std::string extract_value( const std::string & src, std::string key )
                {
                    try
                    {
                        const ncbi::String input ( src );
                        ncbi::JSONObjectRef obj = ncbi::JSON::parseObject ( input );
                        const ncbi::String name ( key );
                        ncbi::JSONValue & val = obj -> getValue( name );
                        return val.toString().toSTLString();
                    }
                    catch( const ncbi::Exception & ex )
                    {
                        std::cerr << "extract_value():" << ex.what() << std::endl;
                        throw;
                    }
                }

                TestCatWriter s_outputs;
        };

    }
}