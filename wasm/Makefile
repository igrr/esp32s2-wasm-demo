CFLAGS := -s WARN_ON_UNDEFINED_SYMBOLS=0 -Os -g -s INITIAL_MEMORY=65536 -s TOTAL_STACK=8192
PROG := hello.wasm

all: $(PROG)
$(PROG): hello.c
	$(CC) $(CFLAGS) $(EXPORTED_RUNTIME_METHODS_ARG) -o $@ $<
$(PROG): Makefile
clean:
	rm -f $(PROG)
.PHONY: all clean
