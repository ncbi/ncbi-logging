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

#include <cassert>
#include <cmath>
#include <cstdbool>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <endian.h>
#include <string>
#include <x86intrin.h>
#define __STDC_FORMAT_MACROS
#include <cinttypes>
#include <collections/Hash.hpp>

#undef memcpy // Code never copies overlapping regions

#if __BYTE_ORDER == __BIG_ENDIAN
#error "Big endian not tested"
#endif

// @TODO: Profile guided optimization should set
// c++20 defines [[likely]] [[unlikely]]
// gcc 9 has __builtin_expect_with_probability
#define UNLIKELY( x ) __builtin_expect ( !!( x ), 0 )

namespace VDB3 {
// Fast, locality preserving, not cryptographically secure hash function.
// Platform depedendent, will not give same results on big-endian.
// References:
// * https://github.com/rurban/smhasher/
// * https://bigdata.uni-saarland.de/publications/p249-richter.pdf

// Some primes between 2^63 and 2^64 for various uses.
static const uint64_t k0 = 0xc3a5c85c97cb3127;
static const uint64_t k1 = 0xb492b66fbe98f273;
static const uint64_t k2 = 0x9ae16a3b2f90404f;
static const uint64_t lowbits = 0xfcffffffffffffffu;
static const unsigned char lowbits128[16] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc};
static const unsigned char keys[4][16] = {
    // head -c 16 /dev/urandom | xxd -i
    {0xe4, 0x64, 0xea, 0xfd, 0x56, 0x61, 0x72, 0x74, 0x61, 0x6e, 0x69, 0x61,
        0x6e, 0x4d, 0x48, 0xf6},
    {0xff, 0x2c, 0x79, 0x82, 0xff, 0xf8, 0xe8, 0x8e, 0x8e, 0xd9, 0xdf, 0xbd,
        0xea, 0xc0, 0x30, 0x4f},
    {0xab, 0x57, 0x46, 0xe4, 0x2b, 0x96, 0xd1, 0x6e, 0x07, 0xd0, 0x43, 0xc7,
        0x24, 0x9b, 0xdf, 0x2e},
    {0x09, 0xbe, 0xf9, 0x7c, 0x49, 0x33, 0x47, 0xd2, 0x44, 0x2d, 0x2c, 0x04,
        0xf0, 0xc8, 0xcd, 0x82}};


static inline uint64_t bigmix2 ( uint64_t hash, uint64_t ll1, uint64_t ll2 )
{
    uint64_t b1 = ( ll2 >> 56 ) << 1;
    ll2 &= lowbits;
    ll1 *= k0;
    ll2 *= k1;
    ll1 = rotr ( ll1, 33 );
    ll2 = rotr ( ll2, 37 );
    ll1 *= k2;
    ll2 *= k0;
    hash ^= ll1;
    hash ^= ll2;
    hash *= k1;
    hash = rotr ( hash, 33 );
    hash += b1;
    return hash;
}

static inline uint64_t mix128 ( size_t len, const char *s )
{
    const __m128i c128_0 = _mm_setzero_si128 ();
    const __m128i c128_1
        = _mm_loadu_si128 ( reinterpret_cast<const __m128i *> ( keys[0] ) );
    const __m128i c128_low
        = _mm_loadu_si128 ( reinterpret_cast<const __m128i *> ( lowbits128 ) );
    const __m128i *s0 = reinterpret_cast<const __m128i *> ( s );
    const __m128i *s1 = reinterpret_cast<const __m128i *> ( s + ( len - 16 ) );
    const auto h1 = _mm_loadu_si128 ( s0 );
    const auto h2 = h1 ^ c128_1;


    const auto hlow = _mm_loadu_si128 ( s1 );
    const auto hlow1 = _mm_and_si128 ( hlow, c128_low );
    const auto hlow2 = _mm_bsrli_si128 ( hlow, 15 );
    const auto hlow3 = _mm_slli_epi64 ( hlow2, 1 );

    auto h5 = _mm_aesenc_si128 ( h2, hlow1 );
    h5 = _mm_aesenc_si128 ( h5, c128_0 );
    const auto h6 = _mm_aesenc_si128 ( h5, hlow3 );
    const auto h7 = _mm_bsrli_si128 ( h6, 8 );
    const auto h8 = h6 ^ h7;

    return static_cast<uint64_t> ( _mm_cvtsi128_si64 ( h8 ) );
}


uint64_t Hash ( const char *s, size_t len ) noexcept
{
    uint64_t hash = len * k0; // maybe seed?

    const __m128i c128_1
        = _mm_loadu_si128 ( reinterpret_cast<const __m128i *> ( keys[0] ) );
    const __m128i c128_2
        = _mm_loadu_si128 ( reinterpret_cast<const __m128i *> ( keys[1] ) );
    const __m128i c128_3
        = _mm_loadu_si128 ( reinterpret_cast<const __m128i *> ( keys[2] ) );
    const __m128i c128_4
        = _mm_loadu_si128 ( reinterpret_cast<const __m128i *> ( keys[3] ) );
    __m128i h1 = c128_1;
    __m128i h2 = c128_2;
    __m128i h3 = c128_3;
    __m128i h4 = c128_4;

    if ( UNLIKELY ( len >= 32 ) ) {
        // High throughput mode
        while ( len >= 64 ) {
            len -= 64;
            const __m128i *s128 = reinterpret_cast<const __m128i *> ( s );

            h1 *= 7;
            h2 *= 7;
            h3 *= 7;
            h4 *= 7;

            auto h5 = _mm_loadu_si128 ( s128 + 0 );
            auto h6 = _mm_loadu_si128 ( s128 + 1 );
            auto h7 = _mm_loadu_si128 ( s128 + 2 );
            auto h8 = _mm_loadu_si128 ( s128 + 3 );

            h1 += h5;
            h2 += h6;
            h3 += h7;
            h4 += h8;

            s += 64;
        }

        if ( len >= 32 ) {
            const __m128i *s128 = reinterpret_cast<const __m128i *> ( s );
            len -= 32;
            h1 = _mm_aesenc_si128 ( h1, h3 );
            h2 = _mm_aesenc_si128 ( h2, h4 );
            h1 += _mm_loadu_si128 ( s128 + 0 );
            h2 += _mm_loadu_si128 ( s128 + 1 );

            s += 32;
        }
        auto h6 = h1;
        h1 = _mm_aesenc_si128 ( h1, h2 );
        h2 = _mm_aesenc_si128 ( h2, h3 );
        h3 = _mm_aesenc_si128 ( h3, h4 );
        h4 = _mm_aesenc_si128 ( h4, h6 );
        h1 = _mm_aesenc_si128 ( h1, h1 );
        h2 = _mm_aesenc_si128 ( h2, h2 );
        h3 = _mm_aesenc_si128 ( h3, h3 );
        h4 = _mm_aesenc_si128 ( h4, h4 );

        h1 = h1 ^ h3;
        h2 = h2 ^ h4;
        h1 = h1 ^ h2;

        h1 ^= _mm_bsrli_si128 ( h1, 8 );

        hash += static_cast<uint64_t> ( _mm_cvtsi128_si64 ( h1 ) );
    }

    assert ( len < 32 );
    len &= 31;

    // Goal is to have only one unpredictable branch and to use the minimum
    // number of cache loads
    switch ( len ) {
    case 0: {
        return hash;
    }
    case 1: {
        uint8_t b1;
        memcpy ( &b1, s, sizeof ( b1 ) );
        hash += static_cast<uint64_t> ( b1 ) * 2u;
        return hash;
    }
    case 2: {
        uint16_t w1;
        memcpy ( &w1, s, sizeof ( w1 ) );
        const uint8_t b1 = static_cast<uint8_t> ( w1 >> 8 );
        w1 &= static_cast<uint16_t> ( lowbits >> 48 );
        hash += static_cast<uint64_t> ( w1 ) * k0;
        hash *= k0;
        hash = rotr ( hash, 27 );
        hash += static_cast<uint64_t> ( b1 ) * 2u;
        return hash;
    }
    case 3: {
        uint16_t w1;
        uint16_t w2;
        memcpy ( &w1, s, sizeof ( w1 ) );
        memcpy ( &w2, s + 1, sizeof ( w2 ) );
        const uint8_t b1 = static_cast<uint8_t> ( w2 >> 8 );
        w2 &= static_cast<uint16_t> ( lowbits >> 48 );
        hash += static_cast<uint64_t> ( w1 + w2 ) * k1;
        hash *= k2;
        hash = rotr ( hash, 28 );
        hash += static_cast<uint64_t> ( b1 ) * 2u;
        return hash;
    }
    case 4: {
        uint32_t l1;
        memcpy ( &l1, s, sizeof ( l1 ) );
        const uint8_t b1 = static_cast<uint8_t> ( l1 >> 24 );
        l1 &= static_cast<uint32_t> ( lowbits >> 32 );
        hash += l1 * k0;
        hash = rotr ( hash, 29 );
        hash += static_cast<uint64_t> ( b1 ) * 2u;
        return hash;
    }
    case 5: {
        uint32_t l1, l2;
        memcpy ( &l1, s, sizeof ( l1 ) );
        memcpy ( &l2, s + 1, sizeof ( l2 ) );
        const uint8_t b1 = static_cast<uint8_t> ( l2 >> 24 );
        l2 &= static_cast<uint32_t> ( lowbits >> 32 );
        hash += l1;
        hash = rotr ( hash, 32 );
        hash += l2;
        hash *= k1;
        hash = rotr ( hash, 30 );
        hash += static_cast<uint64_t> ( b1 ) * 2u;
        return hash;
    }
    case 6: {
        uint32_t l1, l2;
        memcpy ( &l1, s, sizeof ( l1 ) );
        memcpy ( &l2, s + 2, sizeof ( l2 ) );
        const uint8_t b1 = static_cast<uint8_t> ( l2 >> 24 );
        l2 &= static_cast<uint32_t> ( lowbits >> 32 );
        hash += l1;
        hash = rotr ( hash, 32 );
        hash += l2;
        hash *= k2;
        hash = rotr ( hash, 31 );
        hash += static_cast<uint64_t> ( b1 ) * 2u;
        return hash;
    }
    case 7: {
        uint32_t l1, l2;
        memcpy ( &l1, s, sizeof ( l1 ) );
        memcpy ( &l2, s + 3, sizeof ( l2 ) );
        const uint8_t b1 = static_cast<uint8_t> ( l2 >> 24 );
        l2 &= static_cast<uint32_t> ( lowbits >> 32 );
        hash += l1;
        hash = rotr ( hash, 32 );
        hash += l2;
        hash *= k0;
        hash = rotr ( hash, 31 );
        hash += static_cast<uint64_t> ( b1 ) * 2u;
        return hash;
    }
    case 8: { // locality preserving for 64 bit ints
        uint64_t ll1;
        memcpy ( &ll1, s, sizeof ( ll1 ) );
        hash ^= rotr ( hash * k2, 31 );
        hash += ( ll1 << 1 );
        hash += ( ll1 >> 56 );
        return hash;
    }
    case 9: {
        uint64_t ll1, ll2;
        memcpy ( &ll1, s, sizeof ( ll1 ) );
        memcpy ( &ll2, s + 1, sizeof ( ll2 ) );
        return bigmix2 ( hash, ll1, ll2 );
    }
    case 10: {
        uint64_t ll1, ll2;
        memcpy ( &ll1, s, sizeof ( ll1 ) );
        memcpy ( &ll2, s + 2, sizeof ( ll2 ) );
        return bigmix2 ( hash, ll1, ll2 );
    }
    case 11: {
        uint64_t ll1, ll2;
        memcpy ( &ll1, s, sizeof ( ll1 ) );
        memcpy ( &ll2, s + 3, sizeof ( ll2 ) );
        return bigmix2 ( hash, ll1, ll2 );
    }
    case 12: {
        uint64_t ll1, ll2;
        memcpy ( &ll1, s, sizeof ( ll1 ) );
        memcpy ( &ll2, s + 4, sizeof ( ll2 ) );
        return bigmix2 ( hash, ll1, ll2 );
    }
    case 13: {
        uint64_t ll1, ll2;
        memcpy ( &ll1, s, sizeof ( ll1 ) );
        memcpy ( &ll2, s + 5, sizeof ( ll2 ) );
        return bigmix2 ( hash, ll1, ll2 );
    }
    case 14: {
        uint64_t ll1, ll2;
        memcpy ( &ll1, s, sizeof ( ll1 ) );
        memcpy ( &ll2, s + 6, sizeof ( ll2 ) );
        return bigmix2 ( hash, ll1, ll2 );
    }
    case 15: {
        uint64_t ll1, ll2;
        memcpy ( &ll1, s, sizeof ( ll1 ) );
        memcpy ( &ll2, s + 7, sizeof ( ll2 ) );
        return bigmix2 ( hash, ll1, ll2 );
    }
    case 16: {
        const __m128i c128_low = _mm_loadu_si128 (
            reinterpret_cast<const __m128i *> ( lowbits128 ) );
        const __m128i *s128 = reinterpret_cast<const __m128i *> ( s );
        h1 = _mm_loadu_si128 ( s128 );
        h2 = _mm_and_si128 ( h1, c128_low );
        h3 = _mm_bsrli_si128 ( h1, 15 );
        h3 = _mm_slli_epi64 ( h3, 1 );

        h2 ^= c128_1;
        h2 = _mm_aesenc_si128 ( h2, c128_2 );
        h1 = _mm_aesenc_si128 ( h2, h3 );
        h1 += _mm_bsrli_si128 ( h1, 8 );

        return hash + static_cast<uint64_t> ( _mm_cvtsi128_si64 ( h1 ) );
    }
    case 17:
    case 18:
    case 19:
    case 20:
    case 21:
    case 22:
    case 23:
    case 24:
    case 25:
    case 26:
    case 27:
    case 28:
    case 29:
    case 30:
    case 31: {
        return hash + mix128 ( len, s );
    }
    default: // Shouldn't be able to reach this
        __builtin_unreachable ();
        return 0;
    }
    __builtin_unreachable ();
    return 0;
}

uint64_t Hash ( const std::string &str ) noexcept
{
    return Hash ( str.data (), str.size () );
}

uint64_t Hash ( float f ) noexcept
{
    // Handle 0.0==-0.0
    float pf = fabsf ( f );
    return Hash ( reinterpret_cast<const char *> ( &pf ), sizeof ( pf ) );
}

uint64_t Hash ( double d ) noexcept
{
    // Handle 0.0==-0.0
    double pd = fabs ( d );
    return Hash ( reinterpret_cast<const char *> ( &pd ), sizeof ( pd ) );
}


} // namespace VDB3
