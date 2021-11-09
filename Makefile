all: hello.wasm
hello.wasm: hello.c
	$(CC) -o $@ $<
clean:
	rm -f hello.wasm
.PHONY: all clean
