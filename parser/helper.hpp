#ifndef _h_helper_
#define _h_helper_

#include "types.h"
#include <cstdint>
#include <string>

namespace NCBI
{
    namespace Logging
    {
        uint8_t month_int( const t_str * s );

        std::string ToString( const t_str & in );
    }
}

#endif
