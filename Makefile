.PHONY: all clean

CC = gcc
CFLAGS += -O2 -Wall -Wextra -Wpedantic -Wshadow -std=c17

all: build build/bftool

build:
	mkdir build

build/bftool: build/bftool.o build/bfclean.o build/bfprint.o build/bfinterpret.o build/bfas.o

build/%.o: src/%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $^

examples/hello_world.bfc:

examples/joke.bfc:

# Compiling of Brainfuck programs into bytecode
%.bfc: %.bf
	build/bftool -co $@ $^

# Compiling of Brainfuck programs into assembly
%.S: %.bfc
	build/bftool -o $@ $^

clean:
	rm -rf brainfuck build/* examples/*.bfc
