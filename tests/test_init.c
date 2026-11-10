// Test khối 2: khởi tạo.
#include "hc128.h"
#include "test_util.h"
#include "vectors/init_state.h"

// W[0..15]: khóa và IV mỗi thứ chép 2 lần, ghép byte theo little-endian.
static void test_expand_copies_key_and_iv(void)
{
    static uint32_t W[HC128_W_WORDS];

    hc128_expand(SET6_V0_KEY, SET6_V0_IV, W);
    ASSERT_EQ_U32(0xf9a65300u, W[0]);   // 4 byte đầu của khóa: 00 53 a6 f9
    ASSERT_EQ_U32(0xf9a65300u, W[4]);   // khóa chép lần 2
    ASSERT_EQ_U32(0x42db740du, W[8]);   // 4 byte đầu của IV: 0d 74 db 42
    ASSERT_EQ_U32(0x42db740du, W[12]);  // IV chép lần 2
}

// Tính tay với khóa, IV toàn 0: W[16] = 0 + 0 + 0 + 0 + 16,
// W[18] = f2(0x10) + 18 = 0x000a0000 + 18.
static void test_expand_zero_by_hand(void)
{
    static uint32_t W[HC128_W_WORDS];

    hc128_expand(SET2_V0_KEY, SET2_V0_IV, W);
    ASSERT_EQ_U32(0x00000010u, W[16]);
    ASSERT_EQ_U32(0x000a0012u, W[18]);
}

// So toàn bộ W với vector trích từ code tham chiếu.
static void test_expand_set2(void)
{
    static uint32_t W[HC128_W_WORDS];

    hc128_expand(SET2_V0_KEY, SET2_V0_IV, W);
    ASSERT_EQ_WORDS(SET2_V0_W, W, HC128_W_WORDS);
}

static void test_expand_set6(void)
{
    static uint32_t W[HC128_W_WORDS];

    hc128_expand(SET6_V0_KEY, SET6_V0_IV, W);
    ASSERT_EQ_WORDS(SET6_V0_W, W, HC128_W_WORDS);
}

// P lấy W[256..767], Q lấy W[768..1279].
static void test_load_tables(void)
{
    static hc128_state state;

    hc128_load_tables(&state, SET6_V0_W);
    ASSERT_EQ_U32(SET6_V0_W[256], state.P[0]);
    ASSERT_EQ_U32(SET6_V0_W[1279], state.Q[511]);
    ASSERT_EQ_WORDS(SET6_V0_W + 256, state.P, HC128_TABLE_WORDS);
    ASSERT_EQ_WORDS(SET6_V0_W + 768, state.Q, HC128_TABLE_WORDS);
}

// Nạp W rồi chạy 1024 bước, so với P, Q trích từ code tham chiếu.
static void test_mix_tables_set2(void)
{
    static hc128_state state;

    hc128_load_tables(&state, SET2_V0_W);
    hc128_mix_tables(&state);
    ASSERT_EQ_WORDS(SET2_V0_P, state.P, HC128_TABLE_WORDS);
    ASSERT_EQ_WORDS(SET2_V0_Q, state.Q, HC128_TABLE_WORDS);
}

static void test_mix_tables_set6(void)
{
    static hc128_state state;

    hc128_load_tables(&state, SET6_V0_W);
    hc128_mix_tables(&state);
    ASSERT_EQ_WORDS(SET6_V0_P, state.P, HC128_TABLE_WORDS);
    ASSERT_EQ_WORDS(SET6_V0_Q, state.Q, HC128_TABLE_WORDS);
}

int main(void)
{
    RUN_TEST(test_expand_copies_key_and_iv);
    RUN_TEST(test_expand_zero_by_hand);
    RUN_TEST(test_expand_set2);
    RUN_TEST(test_expand_set6);
    RUN_TEST(test_load_tables);
    RUN_TEST(test_mix_tables_set2);
    RUN_TEST(test_mix_tables_set6);
    return test_summary();
}
