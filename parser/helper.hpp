#pragma once

#include "types.h"
#include "LogLinesInterface.hpp"

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

        class OneWriterManyReadersQueue
        {   
            public :
                OneWriterManyReadersQueue( size_t limit );

                // called by the line-splitter aka the producer
                // returns false if the queue is full
                bool enqueue( const std::string & s );

                // called by the worker-threads aka the consumers
                bool dequeue( std::string & s, size_t & line_nr );

                void close() { m_open.store( false ); }
                bool is_open() const { return m_open.load(); }
                size_t m_max;

            private :
                std::queue< std::string > m_queue;
                std::atomic< bool > m_open;
                std::mutex m_mutex;
                size_t m_limit;
                size_t m_line_nr;
        };

        class OutputQueue
        {
            typedef std::pair< std::string, LogLinesInterface::Category > output_pair;

            public :
                OutputQueue( size_t limit );

                // called by the worker-threads to put output in
                // returns false if the queue is full
                bool enqueue( const std::string & s, LogLinesInterface::Category cat );

                // called by the worker-threads aka the consumers
                bool dequeue( std::string & s, LogLinesInterface::Category &cat );

                void close() { m_open.store( false ); }
                bool is_open() const { return m_open.load(); }

            private :
                std::queue< output_pair > m_queue;
                std::atomic< bool > m_open;
                std::mutex m_mutex;
                size_t m_limit;
        };

    }
}
