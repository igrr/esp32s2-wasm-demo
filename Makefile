CFLAGS := -s WARN_ON_UNDEFINED_SYMBOLS=0 -Os -g -s INITIAL_MEMORY=65536 -s TOTAL_STACK=8192

all: hello.wasm
hello.wasm: hello.c
	$(CC) $(CFLAGS) $(EXPORTED_RUNTIME_METHODS_ARG) -o $@ $<
clean:
	rm -f hello.wasm
.PHONY: all clean
