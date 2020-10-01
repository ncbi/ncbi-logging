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

            typedef enum {
                accession = LastMemberId+1,
                filename,
                extension
            } URL_Members;

            typedef enum { acc_before = 0, acc_inside, acc_after } eAccessionMode;

            virtual void set( URL_Members m, const t_str & );
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
