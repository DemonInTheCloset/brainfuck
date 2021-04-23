.PHONY: all clean

CFLAGS+=-Wall -Wextra -std=c17 -Wpedantic

all: bftool

build:
	mkdir build

bftool: build

clean:
	rm -rf build
