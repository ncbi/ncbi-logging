#pragma once

#include <string>

#include "types.h"

#include <ncbi/json.hpp>

namespace NCBI
{
    namespace Logging
    {
        class FormatterInterface
        {
        public:
            virtual ~FormatterInterface() = 0;

            // formats, outputs to s and forgets the data
            virtual std::stringstream & format( std::stringstream & s ) = 0;

            // t_str may have '\' inserted by the original input format (t.str.escaped == true), 
            // these will be removed before conversion into the implementation-specific output format
            // TODO: decide if throw on invald UTF-8 in value
            virtual void addNameValue( const std::string & name, const t_str & value ) = 0;   // {"name":"value"}
            virtual void addNameValue( const std::string & name, int64_t value ) = 0;       // {"name":"value"}
            //TODO: addNameValue( const std::string & name, const std::string & value ) 
        protected:
            std::string & unEscape( const t_str & value, std::string & str ) const; // appends to the end of str, returns updated str
        };

        class JsonLibFormatter : public FormatterInterface
        {
        public:
            JsonLibFormatter();
            virtual ~JsonLibFormatter();

            virtual std::stringstream & format( std::stringstream & );

            virtual void addNameValue( const std::string & name, const t_str & value ); // may throw
            virtual void addNameValue( const std::string & name, int64_t value );    

        private:
            ncbi :: JSONObjectRef j;
        };

        class JsonFastFormatter : public FormatterInterface
        {
        public:
            virtual ~JsonFastFormatter();

            virtual std::stringstream & format( std::stringstream & );

            virtual void addNameValue( const std::string & name, const t_str & value ); 
            virtual void addNameValue( const std::string & name, int64_t value );    

        private:
            std::string escape( const std::string & s ) const; // Json-escape; make a static function?
        };
    }
}

