.PHONY: all clean fresh

CFLAGS+=-Wall -Wextra -Wpedantic -std=c17

all: bftool

fresh: clean all

bftool: bfclean.o bfprint.o

bfclean.o:

bfprint.o: bfclean.o

clean:
	rm -rf *.o bftool
