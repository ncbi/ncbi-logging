#pragma once

#include <string>

#include "ReceiverInterface.hpp"
#include "ParserInterface.hpp"

namespace NCBI
{
    namespace Logging
    {
        struct MSGReceiver : public ReceiverInterface
        {
            MSGReceiver( FormatterRef fmt );
            typedef enum {
                client = LastMemberId+1,
                server,
                host
            } MSG_Members;

            virtual void set( MSG_Members m, const t_str & );
            virtual Category post_process( void ) { return cat_good; };

            std::string path_for_url_parser;
        };

        class MSGParseBlock : public ParseBlockInterface
        {
        public:
            MSGParseBlock( MSGReceiver & receiver );
            virtual ~MSGParseBlock();

            virtual ReceiverInterface & GetReceiver() { return m_receiver; }
            virtual bool format_specific_parse( const char * line, size_t line_size );

            void * m_sc;
            MSGReceiver & m_receiver;
        };
    }
}
