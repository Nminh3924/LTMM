/*
 * Prototypes of the eSTREAM API functions that the HC-128 reference code
 * defines. Written for this project; not part of the reference code.
 */
#ifndef ESTREAM_FUNCTIONS
#define ESTREAM_FUNCTIONS

void ECRYPT_init(void);

/* keysize and ivsize are in bits; HC-128 uses 128 for both. */
void ECRYPT_keysetup(ECRYPT_ctx *ctx, const u8 *key, u32 keysize, u32 ivsize);

void ECRYPT_ivsetup(ECRYPT_ctx *ctx, const u8 *iv);

/* action: 0 = encrypt, 1 = decrypt (the operation is the same). */
void ECRYPT_process_bytes(int action, ECRYPT_ctx *ctx, const u8 *input,
                          u8 *output, u32 msglen);

#endif
