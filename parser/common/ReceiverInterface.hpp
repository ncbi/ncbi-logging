#pragma once

#include <cstring>
#include <memory>

#include "types.h"

namespace NCBI
{
    namespace Logging
    {
        class FormatterInterface;
        typedef enum { acc_before = 0, acc_inside, acc_after } eAccessionMode;

        typedef struct t_request
        {
            t_str method;
            t_str path;
            t_str vers;
            t_str accession;
            t_str filename;
            t_str extension;

            eAccessionMode accession_mode; // private to the GCP and AWS parser
        } t_request;

        inline void InitRequest( t_request & r ) { memset( & r, 0, sizeof ( r ) ); }

        struct ReceiverInterface
        {
            typedef std::shared_ptr<FormatterInterface> FormatterRef;

            ReceiverInterface( FormatterRef p_fmt );

            virtual ~ReceiverInterface();

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
                request,
                LastMemberId = request
            } Members;

            void set( Members m, const t_str & v ); // for request, use the method below
            void setRequest( const t_request & r );

            void setMember( const char * mem, const t_str & v );

            void reportField( const char * message );

            virtual void post_process( void ) { }

            FormatterInterface & GetFormatter();

        protected:
            FormatterRef m_fmt;
            Category m_cat;
        };

    }
}

