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

        // this does not test that s only contains valid signed digits aka: [-+]?[0-9]+
        int64_t ToInt64( const t_str & s );

        std::string ToString( const t_str & in );
    }
}

#endif
