#pragma once

#include "LogLinesInterface.hpp"

#include <fstream>
#include <sstream>
#include <vector>
#include <mutex>

namespace NCBI
{
    namespace Logging
    {
        class CatCounter
        {
            public :
                CatCounter();

                void count( LogLinesInterface::Category cat );

                unsigned long int get_total( void ) const { return total; }
                unsigned long int get_cat_count( LogLinesInterface::Category cat ) const;

            private :
                unsigned long int total;
                std::vector< unsigned long int > cat_counts;
        };

        class CatWriterInterface
        {
        public:
            virtual ~CatWriterInterface() = 0;

            virtual void write( LogLinesInterface::Category cat, const std::string & s ) = 0;
        };

        class FileCatWriter : public CatWriterInterface
        {
        public:
            FileCatWriter( const std::string & basename );
            virtual ~FileCatWriter();

            virtual void write( LogLinesInterface::Category cat, const std::string & s );

            const CatCounter& getCounter() const { return ctr; }

        private:
            std::ofstream review, good, bad, ugly;
            CatCounter ctr;
            //TODO: conside placing this object on a separate thread, or
            // having one writer per category, running on a separate thread.
            std::mutex mut; 
        };

        class StringCatWriter : public CatWriterInterface
        {
        public:
            StringCatWriter() {}
            virtual ~StringCatWriter();

            virtual void write( LogLinesInterface::Category cat, const std::string & s );

            const std::string get_review() { return review . str(); }
            const std::string get_good()   { return good . str(); }
            const std::string get_bad()    { return bad . str(); }
            const std::string get_ugly()   { return ugly . str(); }

        private:
            std::stringstream review, good, bad, ugly;
            std::mutex mut;
        };

    }
}
