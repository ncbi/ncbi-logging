#pragma once

#include "ReceiverInterface.hpp"

#include <fstream>
#include <sstream>
#include <vector>

namespace NCBI
{
    namespace Logging
    {
        class CatCounter
        {
            public :
                CatCounter();

                void count( ReceiverInterface::Category cat );

                unsigned long int get_total( void ) const { return total; }
                unsigned long int get_cat_count( ReceiverInterface::Category cat ) const;

                std::string report( FormatterInterface & fmt ) const;

            private :
                unsigned long int total;
                std::vector< unsigned long int > cat_counts;
        };

        class CatWriterInterface
        {
        public:
            virtual ~CatWriterInterface() = 0;

            virtual void write( ReceiverInterface::Category cat, const std::string & s ) = 0;
        };

        class FileCatWriter : public CatWriterInterface
        {
        public:
            FileCatWriter( const std::string & basename );
            virtual ~FileCatWriter();

            virtual void write( ReceiverInterface::Category cat, const std::string & s );

            const CatCounter& getCounter() const { return ctr; }
            static const std::string extension;

        private:
            std::ofstream review, good, bad, ugly;
            CatCounter ctr;
        };

        class StringCatWriter : public CatWriterInterface
        {
        public:
            StringCatWriter() {}
            virtual ~StringCatWriter();

            virtual void write( ReceiverInterface::Category cat, const std::string & s );

            const std::string get_review() { return review . str(); }
            const std::string get_good()   { return good . str(); }
            const std::string get_bad()    { return bad . str(); }
            const std::string get_ugly()   { return ugly . str(); }

        private:
            std::stringstream review, good, bad, ugly;
        };

    }
}
