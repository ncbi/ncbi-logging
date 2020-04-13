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

#include <cstdint>
#include <string>

#define ATTRPURE __attribute__ ( ( pure ) )
#define ATTRPUREWARNUNUSED __attribute__ ( ( pure, warn_unused_result ) )
#define ATTRWARNUNUSED __attribute__ ( ( warn_unused_result ) )
#define ATTRCONST __attribute__ ( ( const ) )

/**
 * High performance hash function
 */
namespace VDB3 {
uint64_t Hash ( const char *s, size_t len ) noexcept ATTRPUREWARNUNUSED;
uint64_t Hash ( const std::string &str ) noexcept ATTRPURE;

uint64_t Hash ( float f ) noexcept ATTRPURE;
uint64_t Hash ( double d ) noexcept ATTRPURE;

static inline uint64_t rotr ( const uint64_t x, int k )
{
    return ( x << ( 64 - k ) | ( x >> k ) );
}

inline uint64_t HashInline ( uint64_t ll1 ) noexcept
{
    static const uint64_t k1 = 0xb492b66fbe98f273;
    // static const uint64_t k2 = 0x9ae16a3b2f90404f;
    uint64_t ll2 = ( ll1 & 0xFFFFFFFFFFFFFFFCU );
    uint64_t hash = rotr ( ll2 * k1, 47 );
    // hash ^= rotr ( hash * k2, 31 );
    hash += ( ll1 << 1 );
    // hash += ( ll1 >> 56 );
    return hash;
}

inline uint64_t Hash ( int i ) noexcept
{
    return HashInline ( static_cast<uint64_t> ( i ) );
}
inline uint64_t Hash ( long int i ) noexcept
{
    return HashInline ( static_cast<uint64_t> ( i ) );
}
inline uint64_t Hash ( unsigned long long l ) noexcept
{
    return HashInline ( l );
}
inline uint64_t Hash ( uint64_t l ) noexcept { return HashInline ( l ); }

} // namespace VDB3
