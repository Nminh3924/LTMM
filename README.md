# HC-128

Cài đặt mã dòng HC-128 bằng C, kèm unit test và test vector cho từng khối.
Bài tập nhóm môn LTMM.

HC-128 là mã dòng do Hongjun Wu thiết kế, thuộc danh mục eSTREAM dành cho
phần mềm. Thuật toán dùng khóa 128 bit và IV 128 bit.

## Cấu trúc thư mục

```
.
├── include/          Giao diện chung của ba khối (hc128.h)
├── src/              Phần cài đặt của nhóm, mỗi khối một file
├── tests/
│   └── vectors/      Test vector
└── tools/            Code tham chiếu của tác giả và công cụ đối chiếu
```

Code trong `tools/` không phải phần cài đặt của nhóm. Xem
[tools/README.md](tools/README.md) để biết nguồn gốc và cách dùng.

## Yêu cầu

- Trình biên dịch C (`gcc` hoặc `clang`) và `make`.
- macOS và Linux dùng trực tiếp. Windows dùng WSL.

## Phân công

| Thành viên | Phần việc | File |
|---|---|---|
| Vũ Nhật Minh (trưởng nhóm) | Hạ tầng chung; khối 2: khởi tạo | `src/hc128_init.c`, `tests/test_init.c` |
| Nguyễn Tuấn Anh | Khối 1: hàm cơ sở | `src/hc128_core.c`, `tests/test_core.c` |
| Nguyễn Đình Sơn | Khối 3: sinh keystream và mã hóa | `src/hc128_stream.c`, `tests/test_stream.c` |

## Tài liệu tham khảo

- Hongjun Wu, "The Stream Cipher HC-128", bản nộp eSTREAM:
  <https://www.ecrypt.eu.org/stream/p3ciphers/hc/hc128_p3.pdf>
- Dự án eSTREAM: <https://en.wikipedia.org/wiki/ESTREAM>
