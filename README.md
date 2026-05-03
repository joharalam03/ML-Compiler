# ML Compiler

![CI](https://github.com/joharalam03/ML-Compiler/actions/workflows/ci.yml/badge.svg)

A C-based translator for a simplified ML-style programming language. Write clean, minimal ML programs — the compiler handles translating, compiling, and running them automatically.

---

## How It Works

When you run `./ml_translator program.ml`:

1. Reads your `.ml` source file
2. Translates it into a temporary C file
3. Compiles the C file using `gcc`
4. Executes the compiled program
5. Cleans up all generated files

No intermediate files are left behind.

Internally, runml is a single-pass transpiler. The lexer/parser walks
the source line-by-line, classifying each line as a function definition,
an assignment, a print, a return, or a function call. Top-level
statements and per-function bodies are stored in arrays. The code
generator then emits an equivalent C source file (`ml-<pid>.c`),
hands it to `gcc` to produce a temporary executable, runs it with
any extra command-line arguments, and removes both the temporary
C file and the compiled binary on the way out. No intermediate files
are left behind.

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

## Testing

The repo ships with 8 sample programs under `tests/`, each paired with a
`.expected` file containing the program's expected stdout. The Makefile
builds and runs all of them in one command:

```bash
make test
```

The same suite runs in CI on every push and pull request, on both
Ubuntu and macOS — see the badge above.

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
y <- 10
print x + y
```

```bash
./ml_translator examples/simple.ml
```

```
15
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

---

## Implementation Notes

One sample test (`sample07.ml` under `tests/known-failures/`) exposes a
limitation in how function-local variables that depend on parameters
are translated. The current implementation pre-computes variable values
during parsing, then writes those values into the generated C as
initializers. For statements like `x <- a * b` *inside* a function,
the parameters `a` and `b` aren't known until the function is called
at runtime, so the parser substitutes `0` and the resulting C function
always returns `0`.

A correct implementation would emit each ml statement as a real C
statement (e.g. `x = a * b;`) and let the C compiler defer evaluation
to runtime. Restructuring around this — the change is described in
the Future Work section — is the obvious next step.

---

## Future Work

If extended, the highest-leverage changes would be:

- Restructure the translator around an ordered statement IR so
  function bodies emit C statements in source order instead of
  pre-computed values (fixes the issue noted in Implementation Notes)
- Replace the loose `int`/`float` typing in the generated C with
  consistent `double` and a runtime helper for "print as integer
  if exact, else 6 decimals"
- Add control flow (`if`, `while`) — the parser is the easy part;
  emitting valid C blocks for nested constructs is where the
  interesting work begins
- Rewrite the single-file source as a small set of modules
  (lexer, parser, codegen, driver) for readability

  ---

## Credits

Built for **CITS2002 (Systems Programming)** at the University of
Western Australia, August 2024.

- [Johar Khan](https://github.com/joharalam03)
- [Muhammad Aman Sohail](https://github.com/amansohail22)