#pragma once

#include "LogLinesInterface.hpp"

#include <fstream>
#include <sstream>

namespace NCBI
{
    namespace Logging
    {
        class ClassificationInterface
        {
        public:
            virtual ~ClassificationInterface() = 0;

            virtual void write( LogLinesInterface::Category cat, const std::string & s ) = 0;
        };

        class FileClassifier : public ClassificationInterface
        {
        public:
            FileClassifier( const std::string & basename );
            virtual ~FileClassifier();

            virtual void write( LogLinesInterface::Category cat, const std::string & s );

        private:
            std::ofstream review, good, bad, ugly;
        };

        class StringClassifier : public ClassificationInterface
        {
        public:
            StringClassifier() {}
            virtual ~StringClassifier();

            virtual void write( LogLinesInterface::Category cat, const std::string & s );

            const std::string get_review() { return review . str(); }
            const std::string get_good()   { return good . str(); }
            const std::string get_bad()    { return bad . str(); }
            const std::string get_ugly()   { return ugly . str(); }

        private:
            std::stringstream review, good, bad, ugly;
        };

#if 0
        class ThreadedFileClassifier : public ClassificationInterface
        {
        public:
            ThreadedFileClassifier( const string & basename ) :
                review  ( ofstream(baseName+".review") ),
                good    ( ofstream(baseName+".good") ),
                bad     ( ofstream(baseName+".bad") ), 
                ugly    ( ofstream(baseName+".unrecog") )
            {
            }

            OutputThread review, good, bad, ugly;

            virtual ~ThreadedFileClassifier()
            {
                review.close();
                good.close();
                bad.close();
                ugly.close();
            }

            virtual void write( Category cat, const string & s )
            {
                switch ( cat )    
                {
                case cat_review : review << s; break;
                // ...
                }
            }
        }
#endif
    }
}
