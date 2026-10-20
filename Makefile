# Build và chạy unit test.
#
#   make test          chạy mọi test có trong tests/
#   make test-core     chỉ chạy test khối 1 (tương tự: test-init, test-stream)
#   make clean         xoá thư mục build

CC ?= cc
CFLAGS ?= -std=c99 -Wall -Wextra -pedantic -O2

HEADERS = include/hc128.h tests/test_util.h
TEST_BINS = $(patsubst tests/%.c,build/%,$(wildcard tests/test_*.c))

.PHONY: test test-core test-init test-stream clean

test: $(TEST_BINS)
ifeq ($(TEST_BINS),)
	@echo "Chưa có file test nào trong tests/."
else
	@status=0; \
	for t in $(TEST_BINS); do \
		echo "Chạy $$t"; \
		./$$t || status=1; \
	done; \
	exit $$status
endif

test-core test-init test-stream: test-%: build/test_%
	./build/test_$*

# Mỗi test gồm file test của nó và các file nguồn cần dùng.
build/test_%: tests/test_%.c $(HEADERS)
	@mkdir -p build
	$(CC) $(CFLAGS) -Iinclude -Itests -o $@ $(filter %.c,$^)

# Khối 2 và 3 gọi hàm của khối 1.
build/test_core: src/hc128_core.c
build/test_init: src/hc128_init.c src/hc128_core.c
build/test_stream: src/hc128_stream.c src/hc128_core.c

clean:
	rm -rf build
