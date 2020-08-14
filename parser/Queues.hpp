#pragma once

#include "types.h"
#include "ReceiverInterface.hpp"

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
                //TODO: experiment with unique_ptr and/or naked pointers
                typedef std::shared_ptr<std::string> value_type;

                OneWriterManyReadersQueue( size_t limit );

                // called by the line-splitter aka the producer
                // returns false if the queue is full
                bool enqueue( value_type s );

                // called by the worker-threads aka the consumers
                value_type dequeue( size_t & line_nr );

                void close() { m_open.store( false ); }
                bool is_open() const { return m_open.load(); }
                size_t m_max;

            private :
                std::queue< value_type > m_queue;
                std::atomic< bool > m_open;
                std::mutex m_mutex;
                size_t m_limit;
                size_t m_line_nr;
        };

        class OutputQueue
        {
            public :
                //TODO: experiment with unique_ptr and/or naked pointers
                typedef std::shared_ptr< std::string > string_type;

                OutputQueue( size_t limit );

                // called by the worker-threads to put output in
                // returns false if the queue is full
                bool enqueue( string_type s, ReceiverInterface::Category cat );

                // called by the worker-threads aka the consumers
                string_type dequeue( ReceiverInterface::Category &cat );

                void close() { m_open.store( false ); }
                bool is_open() const { return m_open.load(); }

            private :
                typedef std::pair< string_type, ReceiverInterface::Category > output_pair;
                std::queue< output_pair > m_queue;
                std::atomic< bool > m_open;
                std::mutex m_mutex;
                size_t m_limit;
        };

    }
}
