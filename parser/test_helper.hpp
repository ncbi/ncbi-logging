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
            std::string extension;

            SRequest& operator= ( const t_request &req )
            {
                server = ToString( req.server );
                method = ToString( req.method );
                path = ToString( req.path );
                vers = ToString( req.vers );
                accession = ToString( req.accession );
                extension = ToString( req.extension );
                return *this;
            }
        };

    }
}
