/*
 * Runs the HC-128 reference code against a keystream test-vector file and
 * reports PASS or FAIL for each vector.
 *
 * Usage: check_reference <vector-file>
 *
 * The file holds "name = ...", "key = ...", "iv = ..." and "keystream = ..."
 * lines; a vector is checked when its keystream line is read.
 */
#include <stdio.h>
#include <string.h>

#include "ecrypt-sync.h"

#define KEY_BYTES 16
#define IV_BYTES 16
#define KEYSTREAM_BYTES 64

/* Parses exactly `len` bytes of hex from `hex`. Returns 1 on success. */
static int parse_hex(const char *hex, u8 *out, size_t len)
{
    size_t i;

    for (i = 0; i < len; i++) {
        unsigned int byte;

        if (sscanf(hex + 2 * i, "%2x", &byte) != 1)
            return 0;
        out[i] = (u8)byte;
    }
    return 1;
}

static void print_hex(const char *label, const u8 *bytes, size_t len)
{
    size_t i;

    printf("  %s", label);
    for (i = 0; i < len; i++)
        printf("%02X", bytes[i]);
    printf("\n");
}

/* The reference code accesses its buffers through u32 pointers, so the
 * buffers passed to it are declared as u32 arrays to keep them aligned. */
static void reference_keystream(const u8 *key, const u8 *iv, u8 *keystream)
{
    static ECRYPT_ctx ctx;
    u32 key_words[KEY_BYTES / 4];
    u32 iv_words[IV_BYTES / 4];
    u32 zeros[KEYSTREAM_BYTES / 4] = {0};
    u32 output[KEYSTREAM_BYTES / 4];

    memcpy(key_words, key, KEY_BYTES);
    memcpy(iv_words, iv, IV_BYTES);

    ECRYPT_keysetup(&ctx, (const u8 *)key_words, KEY_BYTES * 8, IV_BYTES * 8);
    ECRYPT_ivsetup(&ctx, (const u8 *)iv_words);
    /* Encrypting zeros yields the keystream itself. */
    ECRYPT_process_bytes(0, &ctx, (const u8 *)zeros, (u8 *)output,
                         KEYSTREAM_BYTES);

    memcpy(keystream, output, KEYSTREAM_BYTES);
}

int main(int argc, char **argv)
{
    FILE *file;
    char line[512];
    char name[128] = "(unnamed)";
    u8 key[KEY_BYTES] = {0};
    u8 iv[IV_BYTES] = {0};
    int checked = 0;
    int failed = 0;

    if (argc != 2) {
        fprintf(stderr, "usage: %s <vector-file>\n", argv[0]);
        return 2;
    }
    file = fopen(argv[1], "r");
    if (file == NULL) {
        perror(argv[1]);
        return 2;
    }

    while (fgets(line, sizeof line, file) != NULL) {
        u8 expected[KEYSTREAM_BYTES];
        u8 actual[KEYSTREAM_BYTES];

        line[strcspn(line, "\r\n")] = '\0';

        if (strncmp(line, "name = ", 7) == 0) {
            snprintf(name, sizeof name, "%s", line + 7);
        } else if (strncmp(line, "key = ", 6) == 0) {
            if (!parse_hex(line + 6, key, KEY_BYTES))
                goto malformed;
        } else if (strncmp(line, "iv = ", 5) == 0) {
            if (!parse_hex(line + 5, iv, IV_BYTES))
                goto malformed;
        } else if (strncmp(line, "keystream = ", 12) == 0) {
            if (!parse_hex(line + 12, expected, KEYSTREAM_BYTES))
                goto malformed;

            reference_keystream(key, iv, actual);
            checked++;
            if (memcmp(expected, actual, KEYSTREAM_BYTES) == 0) {
                printf("PASS  %s\n", name);
            } else {
                failed++;
                printf("FAIL  %s\n", name);
                print_hex("expected: ", expected, KEYSTREAM_BYTES);
                print_hex("actual:   ", actual, KEYSTREAM_BYTES);
            }
        }
    }
    fclose(file);

    printf("%d vector(s) checked, %d failed\n", checked, failed);
    return (checked > 0 && failed == 0) ? 0 : 1;

malformed:
    fprintf(stderr, "malformed line in %s: %s\n", argv[1], line);
    fclose(file);
    return 2;
}
