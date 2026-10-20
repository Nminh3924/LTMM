// HC-128: giao diện chung cho ba khối.
#ifndef HC128_H
#define HC128_H

#include <stddef.h>
#include <stdint.h>

#define HC128_KEY_BYTES   16
#define HC128_IV_BYTES    16
#define HC128_TABLE_WORDS 512   // số từ của mỗi bảng P, Q
#define HC128_W_WORDS     1280  // số từ của mảng W

typedef struct {
    uint32_t P[HC128_TABLE_WORDS];
    uint32_t Q[HC128_TABLE_WORDS];
    uint32_t counter;          // bước i mod 1024
    uint8_t buffer[4];         // byte keystream còn dư
    unsigned int buffer_left;  // số byte dư chưa dùng
} hc128_state;

// Khối 1: hàm cơ sở (src/hc128_core.c)

// Quay trái, quay phải n bit, 1 <= n <= 31.
uint32_t hc128_rotl(uint32_t x, unsigned int n);
uint32_t hc128_rotr(uint32_t x, unsigned int n);

uint32_t hc128_f1(uint32_t x);
uint32_t hc128_f2(uint32_t x);
uint32_t hc128_g1(uint32_t x, uint32_t y, uint32_t z);
uint32_t hc128_g2(uint32_t x, uint32_t y, uint32_t z);

// h1 tra bảng Q, h2 tra bảng P.
uint32_t hc128_h1(const uint32_t Q[HC128_TABLE_WORDS], uint32_t x);
uint32_t hc128_h2(const uint32_t P[HC128_TABLE_WORDS], uint32_t x);

// Đổi 4 byte <-> 1 từ, little-endian.
uint32_t hc128_bytes_to_word(const uint8_t bytes[4]);
void hc128_word_to_bytes(uint32_t word, uint8_t bytes[4]);

// Khối 2: khởi tạo (src/hc128_init.c)

// Bước 1: mở rộng khóa và IV thành W.
void hc128_expand(const uint8_t key[HC128_KEY_BYTES],
                  const uint8_t iv[HC128_IV_BYTES],
                  uint32_t W[HC128_W_WORDS]);

// Bước 2: nạp W vào P và Q.
void hc128_load_tables(hc128_state *state, const uint32_t W[HC128_W_WORDS]);

// Bước 3: chạy 1024 bước xáo trộn.
void hc128_mix_tables(hc128_state *state);

// Chạy đủ 3 bước, đặt lại bộ đếm.
void hc128_init(hc128_state *state,
                const uint8_t key[HC128_KEY_BYTES],
                const uint8_t iv[HC128_IV_BYTES]);

// Khối 3: keystream và mã hóa (src/hc128_stream.c)

// Sinh 1 từ keystream.
uint32_t hc128_next_word(hc128_state *state);

// Sinh len byte keystream.
void hc128_keystream(hc128_state *state, uint8_t *out, size_t len);

// out = in XOR keystream, dùng cho cả mã hóa và giải mã.
void hc128_crypt(hc128_state *state, const uint8_t *in, uint8_t *out,
                 size_t len);

#endif
