/*
 * Minimal stand-in for eSTREAM's ecrypt-portable.h, providing only what the
 * HC-128 reference code uses. Written for this project; not part of the
 * reference code.
 */
#ifndef ECRYPT_PORTABLE
#define ECRYPT_PORTABLE

#include <stdint.h>

typedef uint8_t u8;
typedef uint32_t u32;

#define ROTR32(v, n) (((u32)(v) >> (n)) | ((u32)(v) << (32 - (n))))

/* The reference code reads key, IV and data through u32 pointers, so this
 * conversion is the identity only on a little-endian machine. */
#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ != __ORDER_LITTLE_ENDIAN__
#error "tools/compat supports little-endian machines only"
#endif
#define U32TO32_LITTLE(v) (v)

#endif
