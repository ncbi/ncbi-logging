#pragma once

#include <string>
#include <sstream>

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
            virtual void addNameValue( const std::string & name, const t_str & value ) = 0;   
            virtual void addNameValue( const std::string & name, int64_t value ) = 0;         
            virtual void addNameValue( const std::string & name, const std::string & value ) = 0;

        protected:
            std::string & unEscape( const t_str & value, std::string & str ) const; // appends to the end of str, returns updated str
        };

        class JsonLibFormatter : public FormatterInterface
        {
        public:
            JsonLibFormatter();
            virtual ~JsonLibFormatter();

            // {"name":"value", ... }
            virtual std::stringstream & format( std::stringstream & );

            // these may throw if 'name' already exists
            virtual void addNameValue( const std::string & name, const t_str & value ); 
            virtual void addNameValue( const std::string & name, int64_t value );    
            virtual void addNameValue( const std::string & name, const std::string & value );

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
            virtual void addNameValue( const std::string & name, const std::string & value );

        private:
            std::string escape( const std::string & s ) const; // Json-escape; make a static function?
            std::vector< std::string > kv;
            std::stringstream ss;
        };
    }
}

