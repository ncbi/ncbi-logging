#pragma once

#include <string>

#include "ReceiverInterface.hpp"
#include "ParserInterface.hpp"

namespace NCBI
{
    namespace Logging
    {
        struct AGENTReceiver : public ReceiverInterface
        {
            AGENTReceiver( FormatterRef fmt );

            typedef enum {
                os = LastMemberId+1,
                tool,
                release,
                phid_compute_env,
                phid_guid,
                phid_session_id,
                libc
            } AGENT_Members;

            virtual void set( AGENT_Members m, const t_str & );
        };

        class AGENTParseBlock : public ParseBlockInterface
        {
        public:
            AGENTParseBlock( AGENTReceiver & receiver );
            virtual ~AGENTParseBlock();

            virtual ReceiverInterface & GetReceiver() { return m_receiver; }
            virtual bool format_specific_parse( const char * line, size_t line_size );

            virtual void SetDebug( bool onOff );

            void * m_sc;
            AGENTReceiver & m_receiver;
        };
    }
}
