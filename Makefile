.PHONY: all clean

CC = gcc
CFLAGS += -Wall -Wextra -Wpedantic -Wshadow -std=c17

ifeq ($(VERBOSE),TRUE)
	HIDE :=
else
	HIDE := @
endif

all: build/bftool

build/bftool: build/bftool.o build/bfclean.o build/bfprint.o build/bfinterpret.o

build/%.o: src/%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $^

clean:
	rm -rf brainfuck build/*
