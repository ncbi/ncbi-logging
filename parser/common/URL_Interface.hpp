#pragma once

#include <string>

#include "ReceiverInterface.hpp"
#include "ParserInterface.hpp"

namespace NCBI
{
    namespace Logging
    {
        struct URLReceiver : public ReceiverInterface
        {
            URLReceiver( FormatterRef fmt );

            void finalize( void );

            std::string m_accession;
            std::string m_filename;
            std::string m_extension;
        };

        class URLParseBlock : public ParseBlockInterface
        {
        public:
            URLParseBlock( URLReceiver & receiver );
            virtual ~URLParseBlock();

            virtual ReceiverInterface & GetReceiver() { return m_receiver; }

            virtual bool format_specific_parse( const char * line, size_t line_size );
            virtual void SetDebug( bool onOff );

            void * m_sc;
            URLReceiver & m_receiver;
        };
    }
}
