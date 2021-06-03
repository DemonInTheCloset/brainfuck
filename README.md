# brainfuck
A number of fun tools for the Brainfuck programming language.

## Install
```
$ make install
```

Compiles project and copies binary (`bftool`) to `~/.local/bin` (make sure it is in your path).

## Usage
```
$ bftool -c -o out.bfc in.bf
```
Compiles the in.bf program into bytecode and saves it to out.bfc

```
$ bftool -i out.bfc
```
Runs the compiled program in the interpreter.

```
$ bftool -o out.s in.bfc
```

Translates the bytecode into x86-64 assembly.

```
$ gcc -no-stlib -o out in.s
```

Compiles the assembly into a binary (use `-g` if you want debugging symbols).

You can find rules to preform all these operations automatically **The rule to compile .s files will overwrite other implicit rules for .s files**.

## Syntax HIghlighting for vim/nvim

[vim-brainfuck](https://github.com/DemonInTheCloset/vim-brainfuck): Simple syntax that turns every non instruction character into comments.

## Source Documentation

### bftool.c

The main program, takes arguments and performs the required file I/O to be able to call the rest of the tools.

### bfdef.h

Contains definitions for the required structures and some helpful functions to break up the instructions.

### bfprint.c

Various print function for brainfuck bytecode and other structures.

### bfas.c

Translation code from brainfuck bytecode into x86-64 assembly.

### bfclean.c

Translation code from brainfuck code into bytecode and some optimizations.

### bfinterpret.c

Bytecode interpreter code.
