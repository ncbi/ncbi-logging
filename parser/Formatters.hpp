#pragma once

#include <string>

#include "types.h"

namespace NCBI
{
    namespace Logging
    {
        class LogLinesInterface;

        class FormatterInterface
        {
        public:
            virtual std::string format( LogLinesInterface & line ) const = 0; // iterate over members of line
            virtual std::string format( const std::string & name, const t_str & value ) const = 0; // {"name":"value"}
        };

        class JsonFormatter : public FormatterInterface
        {
        public:
            virtual std::string format( LogLinesInterface & line ) const;
            virtual std::string format( const std::string & name, const t_str & value ) const;

        private:
            std::string Escape( const t_str & s ) const;
        };

    }
}

