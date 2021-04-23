.PHONY: all clean

CFLAGS+=-Wall -Wextra -std=c17 -Wpedantic

all: bftool

bftool: bftool.o bfclean.o bfprint.o

bfclean.o: bfdef.h bfclean.h

bfprint.o: bfdef.h bfprint.h

clean:
	rm -rf build
