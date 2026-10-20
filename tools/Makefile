# Builds the HC-128 reference code and checks it against the eSTREAM vectors.
#
#   make check    build and run the check
#   make vectors  regenerate ../tests/vectors/init_state.h
#   make clean    remove build output

CC ?= cc
CFLAGS ?= -O2

REFERENCE = reference/hc-128
VECTORS = ../tests/vectors/estream_keystream.txt
INIT_STATE = ../tests/vectors/init_state.h

build/check_reference: check_reference.c $(REFERENCE)/hc-128.c \
		$(REFERENCE)/ecrypt-sync.h compat/ecrypt-portable.h \
		compat/estream-functions.h
	mkdir -p build
	$(CC) $(CFLAGS) -Icompat -I$(REFERENCE) -o $@ \
		check_reference.c $(REFERENCE)/hc-128.c

build/dump_state: dump_state.c $(REFERENCE)/hc-128.c \
		$(REFERENCE)/ecrypt-sync.h compat/ecrypt-portable.h \
		compat/estream-functions.h
	mkdir -p build
	$(CC) $(CFLAGS) -Icompat -I$(REFERENCE) -o $@ \
		dump_state.c $(REFERENCE)/hc-128.c

.PHONY: check vectors clean

check: build/check_reference
	./build/check_reference $(VECTORS)

vectors: build/dump_state
	./build/dump_state > $(INIT_STATE).tmp || { rm -f $(INIT_STATE).tmp; exit 1; }
	mv $(INIT_STATE).tmp $(INIT_STATE)

clean:
	rm -rf build
