# Công cụ đối chiếu

Thư mục này chứa code tham chiếu HC-128 của tác giả thuật toán và chương trình
kiểm tra đi kèm. Nhóm dùng code tham chiếu làm chuẩn đối chiếu khi kiểm thử.
Nó không thuộc phần cài đặt của nhóm; phần đó nằm ở `src/`.

## Nội dung

| Đường dẫn | Nội dung | Người viết |
|---|---|---|
| `reference/hc-128/` | Code tham chiếu HC-128, giữ nguyên bản gốc | Hongjun Wu |
| `compat/` | Các định nghĩa tối thiểu của eSTREAM để biên dịch được code tham chiếu | Nhóm |
| `check_reference.c` | Chạy code tham chiếu với các test vector và báo PASS/FAIL | Nhóm |
| `dump_state.c` | Sinh vector trung gian W, P, Q vào `../tests/vectors/init_state.h` | Nhóm |
| `Makefile` | Lệnh biên dịch và chạy kiểm tra | Nhóm |

## Kiểm tra code tham chiếu

```sh
make check
```

Chương trình cho code tham chiếu sinh 64 byte keystream với từng cặp khóa và IV
trong `../tests/vectors/estream_keystream.txt`, rồi so với keystream đã công bố.
Kết quả mong đợi:

```
PASS  Set 2, vector# 0
PASS  Set 6, vector# 0
PASS  Set 6, vector# 1
PASS  Set 6, vector# 2
PASS  Set 6, vector# 3
5 vector(s) checked, 0 failed
```

## Sinh vector trung gian

```sh
make vectors
```

Lệnh này sinh lại `../tests/vectors/init_state.h`, dùng cho test khối 2 và
khối 3. File có hai vector: Set 2 vector# 0 (khóa và IV toàn 0) và Set 6
vector# 0. Mỗi vector gồm khóa, IV, mảng W, bảng P và Q sau 1024 bước khởi tạo,
và 64 byte keystream đầu. Bảng P, Q ngay sau khi nạp W chính là `W[256..767]`
và `W[768..1279]`.

Cách tính:

- W tính theo công thức trong đặc tả.
- P, Q: nạp W vào bảng rồi chạy hàm `setup_update` của code tham chiếu, kết quả
  phải trùng với bảng do `ECRYPT_ivsetup` tạo ra.
- Keystream do code tham chiếu sinh, phải trùng với vector đã công bố.

Có chỗ nào lệch thì chương trình báo lỗi và giữ nguyên file cũ. CI chạy lại
lệnh này và báo lỗi nếu file trong repo khác với kết quả sinh ra.

## Nguồn gốc

### Code tham chiếu

Hai file trong `reference/hc-128/` là bản cài đặt do tác giả thuật toán,
Hongjun Wu, nộp cho dự án eSTREAM. Chúng được lấy từ bản sao của bộ SUPERCOP
trên GitHub, phiên bản `200701b`, và không bị sửa đổi:

<https://github.com/jedisct1/supercop/tree/b36adb42afa53ce4ec9c804eaf8296ad86233b01/crypto_stream/hc128/e/hc-128/200701b/e>

Ngày lấy: 2026-10-09. Mã băm SHA-256 để kiểm tra file còn nguyên bản:

```
96fb9e0fdc2533fd309d6fdf19f8cfa212cdb83c2b467fd1b3b608be19bddd7e  hc-128.c
4d3bba52d7e1ac44c30354ac9c3a2bb2847dcaad1a5b57c5d8bdc372631d60d4  ecrypt-sync.h
```

Nguồn không kèm file giấy phép. Hai file này được giữ lại chỉ để đối chiếu
kết quả trong phạm vi bài tập.

### Test vector

Năm test vector trong `../tests/vectors/estream_keystream.txt` thuộc bộ test
vector đã kiểm chứng của eSTREAM (Set 2 vector 0 và Set 6 vector 0 đến 3). Chúng
được chép lại từ bộ kiểm thử của thư viện Bouncy Castle:

<https://github.com/bcgit/bc-java/blob/main/core/src/test/java/org/bouncycastle/crypto/test/HCFamilyTest.java>

## Khác biệt giữa code tham chiếu và đặc tả

Code tham chiếu là bản tối ưu tốc độ, nên cấu trúc của nó khác với mô tả trong
bài báo đặc tả thuật toán:

- Bảng P và Q gộp thành một mảng `T[1024]`: `P[i] = T[i]`, `Q[i] = T[512 + i]`.
- Mảng W không được giữ đủ 1280 phần tử mà tính ngay trong `T`.
- Có thêm hai bộ đệm `X[16]`, `Y[16]`; mỗi lần gọi chạy 16 bước liên tiếp.
- Code đọc khóa, IV và dữ liệu qua con trỏ `u32`, nên chỉ chạy đúng trên máy
  little-endian. `compat/ecrypt-portable.h` báo lỗi biên dịch trên máy khác.
