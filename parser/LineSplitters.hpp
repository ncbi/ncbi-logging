#pragma once

#include <string>
#include <iostream>

namespace NCBI
{
    namespace Logging
    {
        class LineSplitterInterface
        {
            public :
                virtual ~LineSplitterInterface() {};

                virtual bool getLine( void ) = 0;
                virtual const char * data( void ) const = 0;
                virtual size_t size( void ) const = 0;
        };

        class StdLineSplitter : public LineSplitterInterface
        {
            public :
                StdLineSplitter( std::istream &is );
                virtual ~StdLineSplitter();

                virtual bool getLine( void );
                virtual const char * data( void ) const;
                virtual size_t size( void ) const;

            private :
                std::istream &m_is;
                std::string m_buffer;
        };

        class CLineSplitter : public LineSplitterInterface
        {
            public :
                CLineSplitter( FILE * f );
                virtual ~CLineSplitter();

                virtual bool getLine( void );
                virtual const char * data( void ) const;
                virtual size_t size( void ) const;

            private :
                FILE * m_f;
                char * m_line;
                size_t m_size;
                size_t m_allocated;
        };

    }
}

