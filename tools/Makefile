# Builds the HC-128 reference code and checks it against the eSTREAM vectors.
#
#   make check    build and run the check
#   make clean    remove build output

CC ?= cc
CFLAGS ?= -O2

REFERENCE = reference/hc-128
VECTORS = ../tests/vectors/estream_keystream.txt

build/check_reference: check_reference.c $(REFERENCE)/hc-128.c \
		$(REFERENCE)/ecrypt-sync.h compat/ecrypt-portable.h \
		compat/estream-functions.h
	mkdir -p build
	$(CC) $(CFLAGS) -Icompat -I$(REFERENCE) -o $@ \
		check_reference.c $(REFERENCE)/hc-128.c

.PHONY: check clean

check: build/check_reference
	./build/check_reference $(VECTORS)

clean:
	rm -rf build
