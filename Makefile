.PHONY: all clean

all: bftool

build:
	mkdir build

bftool: build

clean:
	rm -rf build
