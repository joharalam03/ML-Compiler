# ML Compiler

A C-based translator for a simplified ML-style programming language. Write clean, minimal ML programs — the compiler handles translating, compiling, and running them automatically.

---

## How It Works

1. Reads your `.ml` source file
2. Translates it into a temporary C file
3. Compiles the C file using `gcc`
4. Executes the compiled program
5. Cleans up all generated files

No intermediate files are left behind.

---

## Requirements

- GCC
- GNU Make
- Linux or macOS

---

## Build

```bash
make
```

To clean build artifacts:

```bash
make clean
```

---

## Usage

```bash
./ml_translator <program.ml> [arg0 arg1 ...]
```

Command-line arguments are accessible inside your `.ml` program as `arg0`, `arg1`, etc.

---

## Language Features

### Variable Assignment

Variables are assigned using `<-`:

```
x <- 10
y <- 3.5
```

### Arithmetic

Supports `+`, `-`, `*`, `/` and parentheses:

```
result <- x + y * 2
```

### Print

```
print result
print x + y
```

### Functions

Functions are defined with `function`, followed by the name and parameters. The body is indented:

```
function add a b
    return a + b

print add(3 5)
```

### Command-Line Arguments

Pass values at runtime and reference them with `arg0`, `arg1`, etc.:

```bash
./ml_translator program.ml 10 20
```

```
print arg0 + arg1
```

---

## Example

`examples/simple.ml`:

```
x <- 5
y <- 3
print x + y
```

```bash
./ml_translator examples/simple.ml
```

```
8
```

---

## Project Structure

```
.
├── src/
│   └── ml_translator.c
├── examples/
│   └── simple.ml
├── Makefile
├── .gitignore
└── README.md
```

---

## Known Limitations

- Identifiers are limited to 12 characters
- No support for conditionals or loops
- No nested function definitions
- Integer and float types only