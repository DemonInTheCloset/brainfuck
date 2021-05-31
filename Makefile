.PHONY: all clean src/clean

all: brainfuck

brainfuck: src/bftool
	cp $^ $@

src/%:
	cd src && $(MAKE) $(patsubst src/%,%,$@)

clean:
	cd src && $(MAKE) clean
	rm -rf brainfuck
