# lisp interpreter

Programming Languages (CS-403), The University of Alabama.
Built by Preston Lumpkins.

## Project 1.5 — conditionals & logic

- adds basic math functions

## layout
src/
    sexpression.h
    main.cpp
test/
    test.cpp
    doctest.h (framework);

Makefile

## Build 
make (builds ./repl)
make test (builds and runs the tests)
make clean (removes built binaries)

without make:
    g++ -std=c++17 -Wall -Wextra src/main.cpp -o repl

## Usage

Run with no input redirected for an interactive prompt:

./repl
=> <expression>
<evaluated expression>
---

End the session with Ctrl-C or pipe / redirect input, which prints each result with no prompt:

    echo '(1 2 3)' | ./repl
    ./repl < input.txt

---

Expressions may span multiple lines, and a line may hold several expressions. 

Malformed input prints an error and the loop continues. 

## Progress

- 1.1 data, reader, printer — completed
- 1.2 quote, eval, accessors — completed
- 1.3 global values & predicates — completed
- 1.4 conditionals & logic — completed
- 1.5 basic math
- next: 1.6