#pragma once

#include "types.h"

#include <cstdint>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <queue>
#include <mutex>
#include <atomic>
#include <utility>

namespace NCBI
{
    namespace Logging
    {
        uint8_t month_int( const t_str * s );

        // this does not test that s only contains valid signed digits aka: [-+]?[0-9]+
        int64_t ToInt64( const t_str & s );

        std::string ToString( const t_str & in );

        std::string ToString( const t_timepoint& t ); // [dd/Mon/yyyy:hh:mm:ss (-|+)oooo]

        inline std::ostream& operator<< (std::ostream& os, const t_str & s)
        {
            os << "\"";
            for ( auto i = 0; i < s.n; ++i )
            {
                switch ( s.p[i] )
                {
                case '\\':
                case '\"':
                    os << '\\';
                    os << s.p[i];
                    break;
                default:
                    if ( s.p[i] < 0x20 )
                    {
                        std::ostringstream temp;
                        temp << "\\u";
                        temp << std::hex << std::setfill('0') << std::setw(4);
                        temp << (int)s.p[i];
                        os << temp.str();
                    }
                    else
                    {
                        os << s.p[i];
                    }
                    break;
                }
            }
            os << "\"";
            return os;
        }

        inline std::ostream& operator<< (std::ostream& os, const t_timepoint & t)
        {
            os << "\"" << ToString( t ) << "\"";
            return os;
        }

    }
}
