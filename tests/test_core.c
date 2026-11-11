
#include <stdint.h>
#include "hc128.h"
#include "test_util.h"

/* Kiem tra ham quay bit */
static void test_rotate(void)
{
    ASSERT_EQ_U32(0x1A2B3C09u, hc128_rotl(0x12345678u, 7));
    ASSERT_EQ_U32(0xF02468ACu, hc128_rotr(0x12345678u, 7));
}

/* Kiem tra hai ham F */
static void test_f(void)
{
    ASSERT_EQ_U32(0xE7FCE6EEu, hc128_f1(0x12345678u));
    ASSERT_EQ_U32(0xA1F78649u, hc128_f2(0x12345678u));
}

/* Kiem tra hai ham G */
static void test_g(void)
{
    uint32_t x = 0x12345678u;
    uint32_t y = 0x9ABCDEF0u;
    uint32_t z = 0x0FEDCBA9u;

    ASSERT_EQ_U32(0x362E9BE8u, hc128_g1(x, y, z));
    ASSERT_EQ_U32(0xC2BD0747u, hc128_g2(x, y, z));
}

/* Kiem tra hai ham H */
static void test_h(void)
{
    uint32_t P[HC128_TABLE_WORDS];
    uint32_t Q[HC128_TABLE_WORDS];

    for (int i = 0; i < HC128_TABLE_WORDS; i++) {
        P[i] = (uint32_t)i * 2u;
        Q[i] = (uint32_t)i;
    }

    ASSERT_EQ_U32(428u, hc128_h1(Q, 0x12345678u));
    ASSERT_EQ_U32(856u, hc128_h2(P, 0x12345678u));
}

/* Kiem tra chuyen doi byte va word */
static void test_bytes_word(void)
{
    const uint8_t bytes[4] = {0x78, 0x56, 0x34, 0x12};
    const uint8_t expected[4] = {0x78, 0x56, 0x34, 0x12};
    uint8_t result[4];

    uint32_t word = hc128_bytes_to_word(bytes);

    ASSERT_EQ_U32(0x12345678u, word);

    hc128_word_to_bytes(word, result);

    ASSERT_EQ_BYTES(expected, result, 4);
}

int main(void)
{
    printf("===== HC-128 CORE TEST =====\n");

    RUN_TEST(test_rotate);
    RUN_TEST(test_f);
    RUN_TEST(test_g);
    RUN_TEST(test_h);
    RUN_TEST(test_bytes_word);

    return test_summary();
}
