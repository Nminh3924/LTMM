#include "hc128.h"

/* Sinh 1 tu keystream 32-bit */
uint32_t hc128_next_word(hc128_state *state)
{
    uint32_t i = state->counter;
    uint32_t j = i & 0x1FF;
    uint32_t s;

    if (i < 512) {
        state->P[j] += hc128_g1(state->P[(j - 3) & 0x1FF],
                                state->P[(j - 10) & 0x1FF],
                                state->P[(j - 511) & 0x1FF]);
        s = hc128_h1(state->Q, state->P[(j - 12) & 0x1FF]) ^ state->P[j];
    } else {
        state->Q[j] += hc128_g2(state->Q[(j - 3) & 0x1FF],
                                state->Q[(j - 10) & 0x1FF],
                                state->Q[(j - 511) & 0x1FF]);
        s = hc128_h2(state->P, state->Q[(j - 12) & 0x1FF]) ^ state->Q[j];
    }

    state->counter = (i + 1) & 0x3FF;
    return s;
}

/* Sinh len byte keystream */
void hc128_keystream(hc128_state *state, uint8_t *out, size_t len)
{
    size_t pos = 0;

    /* Lay not byte du tu lan truoc neu co */
    while (state->buffer_left > 0 && pos < len) {
        out[pos++] = state->buffer[4 - state->buffer_left];
        state->buffer_left--;
    }

    /* Sinh tung khoi 4 byte */
    while (pos + 4 <= len) {
        uint32_t w = hc128_next_word(state);
        hc128_word_to_bytes(w, &out[pos]);
        pos += 4;
    }

    /* Xu ly phan byte le con lai */
    if (pos < len) {
        uint32_t w = hc128_next_word(state);
        size_t rem = len - pos;

        hc128_word_to_bytes(w, state->buffer);
        for (size_t k = 0; k < rem; k++) {
            out[pos + k] = state->buffer[k];
        }
        state->buffer_left = 4 - (unsigned int)rem;
    }
}

/* Ma hoa / giai ma: out = in XOR keystream */
void hc128_crypt(hc128_state *state, const uint8_t *in, uint8_t *out,
                 size_t len)
{
    uint8_t ks[64];
    size_t pos = 0;

    while (pos < len) {
        size_t chunk = len - pos;
        if (chunk > sizeof(ks)) {
            chunk = sizeof(ks);
        }

        hc128_keystream(state, ks, chunk);
        for (size_t i = 0; i < chunk; i++) {
            out[pos + i] = in[pos + i] ^ ks[i];
        }
        pos += chunk;
    }
}