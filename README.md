# lisp interpreter

Programming Languages (CS-403), The University of Alabama.
Built by Preston Lumpkins.

## Project 1.7 — local functino calls

- sets functions
- gathers parameters and pushes onto the local stack
- recursively evaluates function body

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

per the requirements, expressions may span multiple lines, and a line may hold several expressions. 

Malformed input prints an error and the loop continues. 

## Progress and Summary

- 1.1 data, reader, printer — completed
- 1.2 quote, eval, accessors — completed
- 1.3 global values & predicates — completed
- 1.4 conditionals & logic — completed
- 1.5 basic math - completed
- 1.6 local env - completed
- 1.7 function calls - completed
- next: 1.8

## Test Output: 

pslum@PrestonsLaptop /cygdrive/d/academics/CS-403/projects/lisp-interpreter-cpp
$ make test
g++ -std=c++17 -Wall -Wextra -I src tests/tests.cpp -o tests/run
./tests/run
[doctest] doctest version is "2.4.11"
[doctest] run with "--help" for options
===============================================================================
[doctest] test cases:  32 |  32 passed | 0 failed | 0 skipped
[doctest] assertions: 170 | 170 passed | 0 failed |
[doctest] Status: SUCCESS!