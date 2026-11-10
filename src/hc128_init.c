// Khối 2: khởi tạo bảng P, Q từ khóa và IV.
#include "hc128.h"

// Phép j ⊟ k trong đặc tả: (j - k) mod 512, không bao giờ âm.
static uint32_t sub512(uint32_t j, uint32_t k)
{
    return (j - k) & 511u;
}

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

void hc128_load_tables(hc128_state *state, const uint32_t W[HC128_W_WORDS])
{
    uint32_t i;

    for (i = 0; i < HC128_TABLE_WORDS; i++) {
        state->P[i] = W[i + 256];
        state->Q[i] = W[i + 768];
    }
}

void hc128_mix_tables(hc128_state *state)
{
    uint32_t *P = state->P;
    uint32_t *Q = state->Q;
    uint32_t i;

    // Cập nhật hết P trước, rồi mới tới Q, vì h2 tra bảng P đã cập nhật.
    for (i = 0; i < HC128_TABLE_WORDS; i++)
        P[i] = (P[i] + hc128_g1(P[sub512(i, 3)], P[sub512(i, 10)],
                                P[sub512(i, 511)])) ^
               hc128_h1(Q, P[sub512(i, 12)]);
    for (i = 0; i < HC128_TABLE_WORDS; i++)
        Q[i] = (Q[i] + hc128_g2(Q[sub512(i, 3)], Q[sub512(i, 10)],
                                Q[sub512(i, 511)])) ^
               hc128_h2(P, Q[sub512(i, 12)]);
}
