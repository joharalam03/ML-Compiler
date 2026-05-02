# ML-Compiler

A C-based translator for a simplified ML-style programming language.

This program reads an `.ml` source file, translates it into temporary C code, compiles the generated C file using `gcc`, executes the compiled program, and then removes the temporary generated files.

## Features

- Variable assignment using `<-`
- Arithmetic expressions with:
  - addition `+`
  - subtraction `-`
  - multiplication `*`
  - division `/`
- `print` statements
- Function definitions
- Function calls
- Function return values
- Command-line arguments using `arg0`, `arg1`, etc.
- Automatic C code generation, compilation, and execution

## Project Structure

```text
.
├── src/
│   └── ml_translator.c
├── examples/
│   └── simple.ml
├── Makefile
├── .gitignore
└── README.md
