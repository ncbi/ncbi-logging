#pragma once

#include <cstring>
#include <memory>

#include "types.h"

namespace NCBI
{
    namespace Logging
    {
        class FormatterInterface;

        struct ReceiverInterface
        {
            typedef std::shared_ptr<FormatterInterface> FormatterRef;

            ReceiverInterface( FormatterRef p_fmt );

            virtual ~ReceiverInterface() = 0 ;

            typedef enum {
                cat_review = 0,
                cat_good,
                cat_bad,
                cat_ugly,
                cat_ignored,
                cat_unknown,
                cat_max } Category;

            Category GetCategory() const { return m_cat; }
            void SetCategory( Category p_cat ) { m_cat = p_cat; }

            typedef enum {
                ip,
                referer,
                unparsed,
                agent,
                method,
                path,
                vers,
                LastMemberId = vers
            } Members;

            void set( Members m, const t_str & v ); // for request, use the method below
            void setMember( const char * mem, const t_str & v );
            void reportField( const char * message );

            virtual bool post_process( void ) { return true; }

            FormatterInterface & GetFormatter();

        protected:
            FormatterRef m_fmt;
            Category m_cat;
        };

    }
}

