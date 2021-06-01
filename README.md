# brainfuck
A number of fun tools for the Brainfuck programming language.

## Compile
```
$ make
```

will create the directory `build` inside it you'll find `bftool`

## Usage
```
$ build/bftool -co out.bfc in.bf
```
Compiles the in.bf program into bytecode and saves it to out.bfc

```
$ build/bftool -i out.bfc
```
Runs the compiled program in the interpreter.
