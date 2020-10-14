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
                vdb_os = LastMemberId+1,
                vdb_tool,
                vdb_release,
                vdb_phid_compute_env,
                vdb_phid_guid,
                vdb_phid_session_id,
                vdb_libc
            } AGENT_Members;

            virtual void set( AGENT_Members m, const t_str & );
            virtual Category post_process( void ) { return cat_good; }
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
