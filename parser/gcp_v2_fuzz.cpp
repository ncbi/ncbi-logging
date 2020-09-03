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

#include <sstream>

#include "GCP_Interface.hpp"
#include "CatWriters.hpp"

using namespace std;
using namespace NCBI::Logging;

extern "C" int
LLVMFuzzerTestOneInput ( const uint8_t * const Data, size_t const Size )
{
    stringstream in;
    in << string( (const char *)Data, Size );

    GCPParseBlockFactory fact;
    StringCatWriter outputs;

    SingleThreadedParser p( in, outputs, fact );
    p . parse();

    return 0;  // Non-zero return values are reserved for future use.
}

