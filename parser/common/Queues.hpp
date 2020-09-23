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
        class OneWriterManyReadersQueue
        {   
            public :
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
