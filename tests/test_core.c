
#include <stdio.h>
#include <assert.h>
#include "hc128.h"

/* Kiem tra ham quay bit */
void test_rotate(void)
{
    assert(hc128_rotl(0x12345678, 7) == 0x1A2B3C09);
    assert(hc128_rotr(0x12345678, 7) == 0xF02468AC);

    printf("[PASS] ROTL va ROTR\n");
}

/* Kiem tra hai ham F */
void test_f(void)
{
    assert(hc128_f1(0x12345678) == 0xE7FCE6EE);
    assert(hc128_f2(0x12345678) == 0xA1F78649);

    printf("[PASS] F1 va F2\n");
}

/* Kiem tra hai ham G */
void test_g(void)
{
    uint32_t x = 0x12345678;
    uint32_t y = 0x9ABCDEF0;
    uint32_t z = 0x0FEDCBA9;

    assert(hc128_g1(x, y, z) == 0x362E9BE8);
    assert(hc128_g2(x, y, z) == 0xC2BD0747);

    printf("[PASS] G1 va G2\n");
}

/* Kiem tra hai ham H */
void test_h(void)
{
    uint32_t P[HC128_TABLE_WORDS];
    uint32_t Q[HC128_TABLE_WORDS];

    int i;

    for (i = 0; i < HC128_TABLE_WORDS; i++)
    {
        P[i] = i * 2;
        Q[i] = i;
    }

    assert(hc128_h1(Q, 0x12345678) == 428);
    assert(hc128_h2(P, 0x12345678) == 856);

    printf("[PASS] H1 va H2\n");
}

/* Kiem tra chuyen doi byte va word */
void test_bytes_word(void)
{
    uint8_t bytes[4] = {0x78, 0x56, 0x34, 0x12};
    uint8_t result[4];

    uint32_t word;

    word = hc128_bytes_to_word(bytes);

    assert(word == 0x12345678);

    hc128_word_to_bytes(word, result);

    assert(result[0] == 0x78);
    assert(result[1] == 0x56);
    assert(result[2] == 0x34);
    assert(result[3] == 0x12);

    printf("[PASS] Bytes va Word\n");
}

int main(void)
{
    printf("===== HC-128 CORE TEST =====\n\n");

    test_rotate();
    test_f();
    test_g();
    test_h();
    test_bytes_word();

    printf("\nTat ca cac test deu thanh cong!\n");

    return 0;
}
