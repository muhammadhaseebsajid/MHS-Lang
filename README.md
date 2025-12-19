
# MHS Programming Language

**MHS** – A minimal, safe, dynamic, self-hosting programming language with elegant `:=` syntax and native performance.

## Features

- Unique `:=` assignment and `val`/`var` immutability
- Dynamic typing with arrays, maps, structs
- Lightweight OOP (structs + methods + `this`)
- Runtime safety (bounds checking, overflow protection)
- Compiles to fast C++ binaries
- Fully bootstrapped

## Quick Start

```bash
g++ mhs_compiler.cpp -o mhs_compiler
./mhs_compiler examples/test.mhs
g++ output.cpp -o app
./app
