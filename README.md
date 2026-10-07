# lisp interpreter

Programming Languages (CS-403), The University of Alabama.
Built by Preston Lumpkins. 

***Some Unit tests were written with the assisstance of Anthropic's Claude Sonnet 5***


## Test Output: 

pslum@PrestonsLaptop /cygdrive/d/academics/CS-403/projects/lisp-interpreter-cpp
make test
g++ -std=c++17 -Wall -Wextra -I src tests/tests.cpp -o tests/run
./tests/run
[doctest] doctest version is "2.4.11"
[doctest] run with "--help" for options
[doctest] test cases:  35 |  35 passed | 0 failed | 0 skipped
[doctest] assertions: 180 | 180 passed | 0 failed |
[doctest] Status: SUCCESS!

## Project 1.7/1.8 — local function calls and def syntactic sugar

#### 1.7
- sets functions
- gathers parameters and pushes onto the local stack
- recursively evaluates function body

#### 1.8
- def acts as syntactic sugar. 

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
- 1.8 def syntactic sugar - completed
- next: ...

