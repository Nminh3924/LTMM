// Khối 2: khởi tạo bảng P, Q từ khóa và IV.
#include "hc128.h"

void hc128_expand(const uint8_t key[HC128_KEY_BYTES],
                  const uint8_t iv[HC128_IV_BYTES],
                  uint32_t W[HC128_W_WORDS])
{
    uint32_t i;

    // W[0..7] là khóa chép 2 lần, W[8..15] là IV chép 2 lần.
    for (i = 0; i < 8; i++) {
        W[i] = hc128_bytes_to_word(key + 4 * (i % 4));
        W[i + 8] = hc128_bytes_to_word(iv + 4 * (i % 4));
    }
    for (i = 16; i < HC128_W_WORDS; i++)
        W[i] = hc128_f2(W[i - 2]) + W[i - 7] + hc128_f1(W[i - 15]) +
               W[i - 16] + i;
}
