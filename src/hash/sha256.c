/*!
 * \file sha256.c
 * \brief Implementation of SHA-256.
 * \author Carl Reinke (implementer)
 * \date 2015-02
 * \copyright http://creativecommons.org/publicdomain/zero/1.0/
 * \see http://csrc.nist.gov/groups/STM/cavp/documents/shs/sha256-384-512.pdf
 */

#include "sha256.h"

#include <assert.h>
#include <limits.h>
#include <string.h>

#ifdef __cplusplus
#define restrict
#endif

static inline uint32_t rotr32( uint32_t value, int shift )
{
    return (value >> shift) | (value << (sizeof value * CHAR_BIT - shift));
}

void sha256_init( struct sha256 *digest )
{
    // The initial hash values are the first 32 bits of the fractional parts of
    // the square roots of the first 8 primes 2..19.
    static const struct sha256 initial_digest =
    {
        {
            0x6a09e667,
            0xbb67ae85,
            0x3c6ef372,
            0xa54ff53a,
            0x510e527f,
            0x9b05688c,
            0x1f83d9ab,
            0x5be0cd19,
        },
    };
    
    memcpy(digest, &initial_digest, sizeof (struct sha256));
}

// The sequence of constant words is the first 32 bits of the fractional parts
// of the cube roots of the first 64 primes 2..311.
static const uint32_t k[] =
{
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

void sha256_feed( struct sha256 *const digest, const uint8_t *restrict block )
{
    uint32_t w[64];
    
    // Compute expanded message blocks using SHA-256 message schedule.
    for (int j = 0; j < 16; ++j)
    {
        w[j] = ((uint32_t)(*block) << 24) |
               ((uint32_t)(*(block + 1)) << 16) |
               ((uint32_t)(*(block + 2)) << 8) |
               ((uint32_t)(*(block + 3)));
        block += 4;
    }
    for (int j = 16; j < 64; ++j)
    {
        const uint32_t s0 = rotr32(w[j - 15], 7) ^ rotr32(w[j - 15], 18) ^ (w[j - 15] >> 3);
        const uint32_t s1 = rotr32(w[j - 2], 17) ^ rotr32(w[j - 2], 19) ^ (w[j - 2] >> 10);
        
        w[j] = s0 + w[j - 7] + s1 + w[j - 16];
    }
    
    // Initialize registers with the intermediate hash values.
    uint32_t a = digest->h[0];
    uint32_t b = digest->h[1];
    uint32_t c = digest->h[2];
    uint32_t d = digest->h[3];
    uint32_t e = digest->h[4];
    uint32_t f = digest->h[5];
    uint32_t g = digest->h[6];
    uint32_t h = digest->h[7];
    
    // Apply the SHA-256 compression function.
    for (int j = 0; j < 64; ++j)
    {
        const uint32_t S1 = rotr32(e, 6) ^ rotr32(e, 11) ^ rotr32(e, 25);
        const uint32_t ch = (e & f) ^ ((~e) & g);
        const uint32_t t1 = h + S1 + ch + k[j] + w[j];
        const uint32_t S0 = rotr32(a, 2) ^ rotr32(a, 13) ^ rotr32(a, 22);
        const uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
        const uint32_t t2 = S0 + maj;
        
        h = g;
        g = f;
        f = e;
        e = d + t1;
        d = c;
        c = b;
        b = a;
        a = t1 + t2;
    }
    
    // Compute the intermediate hash values.
    digest->h[0] += a;
    digest->h[1] += b;
    digest->h[2] += c;
    digest->h[3] += d;
    digest->h[4] += e;
    digest->h[5] += f;
    digest->h[6] += g;
    digest->h[7] += h;
}

void sha256_stream_init( struct sha256_stream *const stream )
{
    sha256_init(&stream->digest);
    
    stream->block_filled = 0;
    stream->byte_length = 0;
}

void sha256_stream_feed( struct sha256_stream *const stream, const uint8_t *data, size_t size )
{
    stream->byte_length += size;
    
    while (size > 0)
    {
        // Avoid copying into the stream's buffer if possible.
        if (stream->block_filled == 0 && size >= 64)
        {
            sha256_feed(&stream->digest, data);
            
            data += 64;
            size -= 64;
        }
        else
        {
            const size_t block_remain = 64 - stream->block_filled;
            const size_t amount = size < block_remain ? size : block_remain;
            
            memcpy(stream->block + stream->block_filled, data, amount);
            stream->block_filled += amount;
            
            if (stream->block_filled == 64)
            {
                stream->block_filled = 0;
                sha256_feed(&stream->digest, stream->block);
            }
            
            data += amount;
            size -= amount;
        }
    }
}

void sha256_stream_finish( struct sha256_stream *const stream )
{
    // The message is appended with one set bit; it is then appended with unset
    // bits until its length modulo 512 is 448; it is then appended with the
    // 64-bit message length (excluding appended bits).
    
    const uint64_t bit_length = stream->byte_length * 8;
    
    // Prepare one set bit and padding of unset bits.
    uint8_t data[64] = { 0x80, /* zeros */ };
    
    size_t block_remain = 64 - stream->block_filled;
    
    // If the block doesn't have enough remaining bytes to fit the message
    // length then it must go in the next block.
    if (block_remain - 1 < 8)
    {
        sha256_stream_feed(stream, data, block_remain);
        
        // The next block is padding of unset bits.
        data[0] = 0x00;
        
        block_remain = 64;
    }
    
    uint8_t *const length_data = data + (block_remain - 8);
    
    for (int i = 0; i < 8; ++i)
        length_data[i] = bit_length >> (64 - 8 - 8 * i);
    
    sha256_stream_feed(stream, data, block_remain);
    
    assert(stream->block_filled == 0);
}

#if defined(TEST_SHA256)
#include <stdio.h>

int main( int argc, char *argv[] )
{
    (void)argc;
    (void)argv;
    
    const char *message[] =
    {
        "",
        "abc",
        "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
        "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopqo",
    };
    const struct sha256 digest[] =
    {
        { { 0xe3b0c442, 0x98fc1c14, 0x9afbf4c8, 0x996fb924, 0x27ae41e4, 0x649b934c, 0xa495991b, 0x7852b855, }, },
        { { 0xba7816bf, 0x8f01cfea, 0x414140de, 0x5dae2223, 0xb00361a3, 0x96177a9c, 0xb410ff61, 0xf20015ad, }, },
        { { 0x248d6a61, 0xd20638b8, 0xe5c02693, 0x0c3e6039, 0xa33ce459, 0x64ff2167, 0xf6ecedd4, 0x19db06c1, }, },
        { { 0x2c4cc4c7, 0xe3a19f0c, 0x58258b55, 0x1d3a4af9, 0x84873cb5, 0x5cc53b93, 0xdd8facf3, 0xc1ba935a, }, },
    };
    
    for (size_t i = 0; i < sizeof message / sizeof *message; ++i)
    {
        const size_t message_length = strlen(message[i]);
        
        for (size_t chunk_size = 1; chunk_size <= message_length; ++chunk_size)
        {
            struct sha256_stream sha256;
            
            sha256_stream_init(&sha256);
            
            const uint8_t *chunk = (const uint8_t *)message[i];
            
            size_t message_remain = message_length;
            
            while (message_remain > 0)
            {
                size_t amount = message_remain < chunk_size ? message_remain : chunk_size;
                
                sha256_stream_feed(&sha256, chunk, amount);
                
                chunk += amount;
                
                message_remain -= amount;
            }
            
            sha256_stream_finish(&sha256);
            
            if (memcmp(&sha256.digest, &digest[i], sizeof (struct sha256)) != 0)
            {
                fprintf(stderr, "Failed.\n");
                fprintf(stderr, "Actual:   %08x %08x %08x %08x %08x %08x %08x %08x\n", sha256.digest.h[0], sha256.digest.h[1], sha256.digest.h[2], sha256.digest.h[3], sha256.digest.h[4], sha256.digest.h[5], sha256.digest.h[6], sha256.digest.h[7]);
                fprintf(stderr, "Expected: %08x %08x %08x %08x %08x %08x %08x %08x\n", digest[i].h[0], digest[i].h[1], digest[i].h[2], digest[i].h[3], digest[i].h[4], digest[i].h[5], digest[i].h[6], digest[i].h[7]);
                return -i;
            }
        }
    }
    
    fprintf(stderr, "Passed.\n");
    return 0;
}
#endif
