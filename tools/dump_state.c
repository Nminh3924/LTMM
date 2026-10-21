/*
 * In ra file header C chứa vector trung gian của bước khởi tạo HC-128,
 * dùng để test khối 2 và khối 3. Với mỗi cặp khóa, IV:
 *
 *   - W tính theo công thức trong đặc tả.
 *   - P, Q sau 1024 bước: nạp W vào bảng rồi chạy hàm setup_update của code
 *     tham chiếu. Kết quả phải trùng với bảng do ECRYPT_ivsetup tạo ra.
 *   - Keystream do code tham chiếu sinh, phải trùng với vector đã công bố.
 *
 * Có chỗ nào lệch thì báo lỗi và trả về 1.
 */
#include <stdio.h>
#include <string.h>

#include "ecrypt-sync.h"

#define KEY_BYTES 16
#define W_WORDS 1280
#define TABLE_WORDS 512
#define KEYSTREAM_BYTES 64

#define F1(x) (ROTR32((x), 7) ^ ROTR32((x), 18) ^ ((x) >> 3))
#define F2(x) (ROTR32((x), 17) ^ ROTR32((x), 19) ^ ((x) >> 10))

// Chép từ tests/vectors/estream_keystream.txt.
static const struct {
    const char *prefix;  // tiền tố tên mảng trong header
    const char *title;
    const char *key;
    const char *iv;
    const char *keystream;
} VECTORS[] = {
    {"SET2_V0", "Set 2, vector# 0: khóa và IV toàn 0",
     "00000000000000000000000000000000",
     "00000000000000000000000000000000",
     "82001573A003FD3B7FD72FFB0EAF63AAC62F12DEB629DCA72785A66268EC758B"
     "1EDB36900560898178E0AD009ABF1F491330DC1C246E3D6CB264F6900271D59C"},
    {"SET6_V0", "Set 6, vector# 0",
     "0053A6F94C9FF24598EB3E91E4378ADD",
     "0D74DB42A91077DE45AC137AE148AF16",
     "2E1ED12A8551C05AF41FF39D8F9DF933122B5235D48FC2A6F20037E69BDBBCE8"
     "05782EFC16C455A4B3FF06142317535EF876104C32445138CB26EBC2F88A684C"},
};

// Đọc đúng len byte từ chuỗi hex. Trả về 1 nếu thành công.
static int parse_hex(const char *hex, u8 *out, size_t len)
{
    size_t i;

    if (strlen(hex) != 2 * len)
        return 0;
    for (i = 0; i < len; i++) {
        unsigned int byte;

        if (sscanf(hex + 2 * i, "%2x", &byte) != 1)
            return 0;
        out[i] = (u8)byte;
    }
    return 1;
}

// Ghép 4 byte thành 1 từ, little-endian.
static u32 load_le32(const u8 *bytes)
{
    return (u32)bytes[0] | (u32)bytes[1] << 8 | (u32)bytes[2] << 16 |
           (u32)bytes[3] << 24;
}

static void expand_w(const u8 *key, const u8 *iv, u32 *W)
{
    u32 i;

    for (i = 0; i < 8; i++)
        W[i] = load_le32(key + 4 * (i % 4));
    for (i = 8; i < 16; i++)
        W[i] = load_le32(iv + 4 * (i % 4));
    for (i = 16; i < W_WORDS; i++)
        W[i] = F2(W[i - 2]) + W[i - 7] + F1(W[i - 15]) + W[i - 16] + i;
}

// Nạp W vào bảng rồi chạy 1024 bước như cuối hàm ECRYPT_ivsetup.
static void mix_with_reference(const u32 *W, ECRYPT_ctx *ctx)
{
    u32 i;

    for (i = 0; i < 2 * TABLE_WORDS; i++)
        ctx->T[i] = W[256 + i];
    for (i = 0; i < 16; i++) {
        ctx->X[i] = ctx->T[TABLE_WORDS - 16 + i];
        ctx->Y[i] = ctx->T[2 * TABLE_WORDS - 16 + i];
    }
    ctx->counter1024 = 0;
    for (i = 0; i < 64; i++)
        setup_update(ctx);
}

static void print_words(const char *prefix, const char *name, const u32 *words,
                        size_t count)
{
    size_t i;

    printf("static const uint32_t %s_%s[%lu] = {", prefix, name,
           (unsigned long)count);
    for (i = 0; i < count; i++)
        printf("%s0x%08lx,", i % 6 == 0 ? "\n    " : " ",
               (unsigned long)words[i]);
    printf("\n};\n\n");
}

static void print_bytes(const char *prefix, const char *name, const u8 *bytes,
                        size_t count)
{
    size_t i;

    printf("static const uint8_t %s_%s[%lu] = {", prefix, name,
           (unsigned long)count);
    for (i = 0; i < count; i++)
        printf("%s0x%02x,", i % 12 == 0 ? "\n    " : " ", (unsigned)bytes[i]);
    printf("\n};\n\n");
}

int main(void)
{
    static ECRYPT_ctx reference, mixed;
    static u32 W[W_WORDS];
    static u32 tables[2 * TABLE_WORDS];
    size_t v;

    printf("/*\n"
           " * Vector trung gian của bước khởi tạo HC-128, cho test khối 2"
           " và khối 3.\n"
           " * File do tools/dump_state sinh ra, không sửa tay."
           " Sinh lại: make -C tools vectors\n"
           " *\n"
           " * Mỗi vector gồm KEY, IV, W, P và Q sau 1024 bước khởi tạo,"
           " và 64 byte\n"
           " * keystream đầu. P, Q ngay sau khi nạp W là W[256..767]"
           " và W[768..1279].\n"
           " */\n"
           "#ifndef INIT_STATE_H\n"
           "#define INIT_STATE_H\n\n"
           "#include <stdint.h>\n\n");

    for (v = 0; v < sizeof VECTORS / sizeof VECTORS[0]; v++) {
        // Bộ đệm kiểu u32 vì code tham chiếu đọc qua con trỏ u32.
        u32 key_words[KEY_BYTES / 4], iv_words[KEY_BYTES / 4];
        u32 zeros[KEYSTREAM_BYTES / 4] = {0}, output[KEYSTREAM_BYTES / 4];
        u8 key[KEY_BYTES], iv[KEY_BYTES];
        u8 expected[KEYSTREAM_BYTES], keystream[KEYSTREAM_BYTES];

        if (!parse_hex(VECTORS[v].key, key, KEY_BYTES) ||
            !parse_hex(VECTORS[v].iv, iv, KEY_BYTES) ||
            !parse_hex(VECTORS[v].keystream, expected, KEYSTREAM_BYTES)) {
            fprintf(stderr, "%s: chuỗi hex sai\n", VECTORS[v].prefix);
            return 1;
        }

        memcpy(key_words, key, KEY_BYTES);
        memcpy(iv_words, iv, KEY_BYTES);
        ECRYPT_keysetup(&reference, (const u8 *)key_words, 128, 128);
        ECRYPT_ivsetup(&reference, (const u8 *)iv_words);
        // Lưu bảng trước khi sinh keystream, vì sinh keystream sửa bảng.
        memcpy(tables, reference.T, sizeof tables);
        ECRYPT_process_bytes(0, &reference, (const u8 *)zeros, (u8 *)output,
                             KEYSTREAM_BYTES);
        memcpy(keystream, output, KEYSTREAM_BYTES);

        expand_w(key, iv, W);
        mix_with_reference(W, &mixed);

        if (memcmp(mixed.T, tables, sizeof tables) != 0) {
            fprintf(stderr, "%s: P, Q tính từ W khác ECRYPT_ivsetup\n",
                    VECTORS[v].prefix);
            return 1;
        }
        if (memcmp(keystream, expected, KEYSTREAM_BYTES) != 0) {
            fprintf(stderr, "%s: keystream khác vector đã công bố\n",
                    VECTORS[v].prefix);
            return 1;
        }

        printf("// %s\n", VECTORS[v].title);
        print_bytes(VECTORS[v].prefix, "KEY", key, KEY_BYTES);
        print_bytes(VECTORS[v].prefix, "IV", iv, KEY_BYTES);
        print_words(VECTORS[v].prefix, "W", W, W_WORDS);
        print_words(VECTORS[v].prefix, "P", tables, TABLE_WORDS);
        print_words(VECTORS[v].prefix, "Q", tables + TABLE_WORDS, TABLE_WORDS);
        print_bytes(VECTORS[v].prefix, "KEYSTREAM", keystream,
                    KEYSTREAM_BYTES);
    }

    printf("#endif\n");
    return 0;
}
