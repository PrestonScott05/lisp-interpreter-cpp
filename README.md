# S-Expression reader

Programming Languages (CS-403), The University of Alabama.
Built by Preston Lumpkins.

## Project 1.3 — global values & predicates

Adds a global environment `rho` and simple assignment.

- `(set name value)` — evaluates `value`, conses `(name value)` onto `rho`,
  returns the value. Never mutates existing bindings; newest binding wins on lookup.
- A symbol now evaluates by looking it up in `rho`; a miss returns the symbol itself.
- Predicates (false is `()`, truth is `T`): `nil?`, `atom?`, `list?`.
- Optional: `not?` (alias of `nil?`), `number?` (true for integer symbols).

In the REPL, bindings persist across lines within a session.

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

End the session with Ctrl-D or pipe / redirect input, which prints each resultl with no prompt:

    echo '(1 2 3)' | ./repl
    ./repl < input.txt

---

Expressions may span multiple lines, and a line may hold several expressions. 

Malformed input prints an error and the loop continues. 

