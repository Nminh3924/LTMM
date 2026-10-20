/*
 * Macro kiểm tra dùng chung cho các file test. Cách dùng:
 *
 *   static void test_f1(void) { ASSERT_EQ_U32(0x02004000u, hc128_f1(1)); }
 *   int main(void) { RUN_TEST(test_f1); return test_summary(); }
 */
#ifndef TEST_UTIL_H
#define TEST_UTIL_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

static int test_count;           // số test đã chạy
static int test_failed;          // số test sai
static int test_check_failures;  // số lần kiểm tra sai, cộng dồn

// So hai từ 32 bit.
#define ASSERT_EQ_U32(expected, actual)                                     \
    do {                                                                    \
        uint32_t e_ = (expected);                                           \
        uint32_t a_ = (actual);                                             \
        if (e_ != a_) {                                                     \
            printf("    %s:%d: mong đợi %08lx, thực tế %08lx\n",           \
                   __FILE__, __LINE__, (unsigned long)e_,                   \
                   (unsigned long)a_);                                      \
            test_check_failures++;                                          \
        }                                                                   \
    } while (0)

// So hai mảng từ 32 bit, báo vị trí sai đầu tiên.
#define ASSERT_EQ_WORDS(expected, actual, count)                            \
    do {                                                                    \
        const uint32_t *e_ = (expected);                                    \
        const uint32_t *a_ = (actual);                                      \
        size_t n_ = (count);                                                \
        size_t i_ = 0;                                                      \
        while (i_ < n_ && e_[i_] == a_[i_])                                 \
            i_++;                                                           \
        if (i_ < n_) {                                                      \
            printf("    %s:%d: từ %lu: mong đợi %08lx, thực tế %08lx\n",   \
                   __FILE__, __LINE__, (unsigned long)i_,                   \
                   (unsigned long)e_[i_], (unsigned long)a_[i_]);           \
            test_check_failures++;                                          \
        }                                                                   \
    } while (0)

// So hai dãy byte, báo vị trí sai đầu tiên.
#define ASSERT_EQ_BYTES(expected, actual, len)                              \
    do {                                                                    \
        const uint8_t *e_ = (expected);                                     \
        const uint8_t *a_ = (actual);                                       \
        size_t n_ = (len);                                                  \
        size_t i_ = 0;                                                      \
        while (i_ < n_ && e_[i_] == a_[i_])                                 \
            i_++;                                                           \
        if (i_ < n_) {                                                      \
            printf("    %s:%d: byte %lu: mong đợi %02x, thực tế %02x\n",   \
                   __FILE__, __LINE__, (unsigned long)i_,                   \
                   (unsigned)e_[i_], (unsigned)a_[i_]);                     \
            test_check_failures++;                                          \
        }                                                                   \
    } while (0)

// Chạy một hàm test rồi in PASS hoặc FAIL kèm tên hàm.
#define RUN_TEST(test_fn)                                                   \
    do {                                                                    \
        int before_ = test_check_failures;                                  \
        test_fn();                                                          \
        test_count++;                                                       \
        if (test_check_failures == before_) {                               \
            printf("PASS  %s\n", #test_fn);                                 \
        } else {                                                            \
            printf("FAIL  %s\n", #test_fn);                                 \
            test_failed++;                                                  \
        }                                                                   \
    } while (0)

// In tổng kết. Trả về 0 nếu mọi test đều qua, dùng làm mã thoát của main.
static int test_summary(void)
{
    printf("%d test, %d sai\n", test_count, test_failed);
    return test_failed == 0 ? 0 : 1;
}

#endif
