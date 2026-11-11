
#include "hc128.h"

/* Quay bit sang trai */
uint32_t hc128_rotl(uint32_t x, unsigned int n)
{
    return (x << n) | (x >> (32 - n));
}

/* Quay bit sang phai */
uint32_t hc128_rotr(uint32_t x, unsigned int n)
{
    return (x >> n) | (x << (32 - n));
}

/* Ham F1 */
uint32_t hc128_f1(uint32_t x)
{
    return hc128_rotr(x, 7)
         ^ hc128_rotr(x, 18)
         ^ (x >> 3);
}

/* Ham F2 */
uint32_t hc128_f2(uint32_t x)
{
    return hc128_rotr(x, 17)
         ^ hc128_rotr(x, 19)
         ^ (x >> 10);
}

/* Ham G1 */
uint32_t hc128_g1(uint32_t x, uint32_t y, uint32_t z)
{
    return (hc128_rotr(x, 10) ^ hc128_rotr(z, 23))
           + hc128_rotr(y, 8);
}

/* Ham G2 */
uint32_t hc128_g2(uint32_t x, uint32_t y, uint32_t z)
{
    return (hc128_rotl(x, 10) ^ hc128_rotl(z, 23))
           + hc128_rotl(y, 8);
}

/* Ham H1: tra bang Q */
uint32_t hc128_h1(const uint32_t Q[HC128_TABLE_WORDS], uint32_t x)
{
    uint32_t x0 = x & 0xFF;
    uint32_t x2 = (x >> 16) & 0xFF;

    return Q[x0] + Q[256 + x2];
}

/* Ham H2: tra bang P */
uint32_t hc128_h2(const uint32_t P[HC128_TABLE_WORDS], uint32_t x)
{
    uint32_t x0 = x & 0xFF;
    uint32_t x2 = (x >> 16) & 0xFF;

    return P[x0] + P[256 + x2];
}

/* Chuyen 4 byte thanh 1 tu 32 bit */
uint32_t hc128_bytes_to_word(const uint8_t bytes[4])
{
    return (uint32_t)bytes[0]
         | ((uint32_t)bytes[1] << 8)
         | ((uint32_t)bytes[2] << 16)
         | ((uint32_t)bytes[3] << 24);
}

/* Chuyen 1 tu 32 bit thanh 4 byte */
void hc128_word_to_bytes(uint32_t word, uint8_t bytes[4])
{
    bytes[0] = word & 0xFF;
    bytes[1] = (word >> 8) & 0xFF;
    bytes[2] = (word >> 16) & 0xFF;
    bytes[3] = (word >> 24) & 0xFF;
}
