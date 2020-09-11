/*===========================================================================
*
*                            PUBLIC DOMAIN NOTICE
*               National Center for Biotechnology Information
*
*  This software/database is a "United States Government Work" under the
*  terms of the United States Copyright Act.  It was written as part of
*  the author's official duties as a United States Government employee and
*  thus cannot be copyrighted.  This software/database is freely available
*  to the public for use. The National Library of Medicine and the U.S.
*  Government have not placed any restriction on its use or reproduction.
*
*  Although all reasonable efforts have been taken to ensure the accuracy
*  and reliability of the software and data, the NLM and the U.S.
*  Government do not and cannot warrant the performance or results that
*  may be obtained by using this software or data. The NLM and the U.S.
*  Government disclaim all warranties, express or implied, including
*  warranties of performance, merchantability or fitness for any particular
*  purpose.
*
*  Please cite the author in any work or product based on this material.
*
* ===========================================================================
*
*/
#pragma once

#include <sstream>

#include "ReceiverInterface.hpp"
#include "CatWriters.hpp"
#include "LineSplitters.hpp"

#ifndef THREAD_NUM
    #define THREAD_NUM 1
#endif

template <class ParseBlockFactory>
int
FuzzHead ( const uint8_t * const Data,
           size_t const Size )
{
    ParseBlockFactory fact;
    NCBI::Logging::StringCatWriter outputs;
    NCBI::Logging::BufLineSplitter input( (const char *)Data, Size );

    if ( THREAD_NUM <= 1 )
    {
        NCBI::Logging::SingleThreadedDriver p( input, outputs, fact.MakeParseBlock() );
        p . parse_all_lines();
    }
    else
    {
        size_t queueLimit = 1000;
        NCBI::Logging::MultiThreadedDriver p( input, outputs, queueLimit, THREAD_NUM, fact );
        p . parse_all_lines();
    }

    return 0;  // Non-zero return values are reserved for future use.
}

