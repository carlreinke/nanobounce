/*!
 * \file sha256.h
 * \brief Implementation of SHA-256.
 * \author Carl Reinke (implementer)
 * \date 2015-02
 * \copyright http://creativecommons.org/publicdomain/zero/1.0/
 */

#ifndef SHA256_H
#define SHA256_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*!
 * \brief A SHA-256 digest.
 */
struct sha256
{
    /*!
     * \brief The SHA-256 digest.
     * 
     * The digest is formed by concatenating (big-endian) <tt>h[0]</tt> through
     * <tt>h[7]</tt>.
     */
    uint32_t h[8];
};

/*!
 * \brief Initialize a SHA-256 digest.
 * \param[in] digest The digest.
 */
extern void sha256_init( struct sha256 *digest );

/*!
 * \brief Digest a block into a SHA-256 digest.
 * \param[in] digest The digest.
 * \param[in] block The 512-bit (64-byte) block to digest.
 */
extern void sha256_feed( struct sha256 *digest, const uint8_t *block );

/*!
 * \brief A SHA-256 digest stream.
 */
struct sha256_stream
{
    /*!
     * \brief The SHA-256 digest.
     * 
     * The digest is intermediate until sha256_stream_finish() is called.
     */
    struct sha256 digest;
    
    /*!
     * \brief The block buffer that stores an undigested partial block.
     * \private
     */
    uint8_t block[64];
    
    /*!
     * \brief The number of bytes of the block buffer that contain valid data.
     * \private
     */
    size_t block_filled;
    
    /*!
     * \brief The total number of bytes digested.
     * \private
     */
    uint64_t byte_length;
};

/*!
 * \brief Initialize a SHA-256 digest stream.
 * \param[in] stream The digest stream.
 */
extern void sha256_stream_init( struct sha256_stream *const stream );

/*!
 * \brief Digest data into a SHA-256 digest stream.
 * \param[in] stream The digest stream.
 * \param[in] data The data to digest.
 * \param[in] size The number of bytes of data to digest.
 */
extern void sha256_stream_feed( struct sha256_stream *stream, const uint8_t *data, size_t size );

/*!
 * \brief Finish a SHA-256 digest stream.
 * \param[in] stream The digest stream.
 */
extern void sha256_stream_finish( struct sha256_stream *const stream );

#ifdef __cplusplus
}
#endif

#endif /* SHA256_H */
