.PHONY: all clean install uninstall

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
	bftool -co $@ $^

# Compiling of Brainfuck programs into assembly
%.s: %.bfc
	bftool -o $@ $^

# Compiling brainfuck assembly into object files
%: %.s
	$(CC) -nostdlib -o $@ $^

clean:
	rm -rf brainfuck build/* examples/*.bfc

install: all
	mkdir -p ~/.local/bin && cp build/bftool -t ~/.local/bin

uninstall:
	rm -f ~/.local/bin/bftool
