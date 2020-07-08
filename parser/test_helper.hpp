#pragma once

#include "types.h"
#include <string>
#include "log_lines.hpp"

namespace NCBI
{
    namespace Logging
    {
        struct SRequest
        {
            std::string server;
            std::string method;
            std::string path;
            std::string vers;
            std::string accession;
            std::string filename;
            std::string extension;

            SRequest& operator= ( const t_request &req )
            {
                server = ToString( req.server );
                method = ToString( req.method );
                path = ToString( req.path );
                vers = ToString( req.vers );
                accession = ToString( req.accession );
                filename = ToString( req.filename );
                extension = ToString( req.extension );
                return *this;
            }
        };

        struct SAgent
        {
            std::string original;
            std::string vdb_os;
            std::string vdb_tool;
            std::string vdb_release;
            std::string vdb_phid_compute_env;
            std::string vdb_phid_guid;
            std::string vdb_phid_session_id;
            std::string vdb_libc;    

            SAgent& operator= ( const t_agent &agt )
            {
                original    = ToString ( agt . original );
                vdb_os      = ToString ( agt . vdb_os );
                vdb_tool    = ToString ( agt . vdb_tool );
                vdb_release = ToString ( agt . vdb_release );
                vdb_phid_compute_env    = ToString ( agt . vdb_phid_compute_env );
                vdb_phid_guid           = ToString ( agt . vdb_phid_guid );
                vdb_phid_session_id     = ToString ( agt . vdb_phid_session_id );
                vdb_libc    = ToString ( agt . vdb_libc );    

                return *this;
            }
        };
    }
}
