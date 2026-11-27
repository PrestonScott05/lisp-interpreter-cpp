# lisp interpreter

Programming Languages (CS-403), The University of Alabama.
Built by Preston Lumpkins. 

***Some Unit tests were written with the assistance of Anthropic's Claude Sonnet 5***

## About
This is a fully functional lisp interpreter with a reader, evaluator, environment handler, special forms, and user defined functions. It follows the specifications set by Professor Donald Yessick - CS-403 - Fall 2026

## layout
src/
    sexpression.h
    main.cpp
tests/
    tests.cpp
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
- Final Project Extension - completed

## Final Project Extensions: 
| feature | example |

- lambda + lexical closures | `((lambda (x) (add x 1)) 41)` |
- begin | `(begin (display "hi") 2)` |
- let / let* | `(let* ((a 1) (b (add a 1))) b)` |
- set! (mutates in place) | `(set! n (add n 1))` |
- while | `(while (lt i 10) (set! i (add i 1)))` |
- strings | `"hello\n"` |
- display / newline | `(display "x = " 5) (newline)` |
- read-line | `(set name (read-line))` |
- load | `(load "lib.lisp")` |
- string?, string-append, string-length | `(string-append "a" "b")` |

Calls in tail position run in a loop instead of recursing, so deep recursion doesn't overflow the stack.
Old `def`/`function` behavior is unchanged.

## Test Output: 

pslum@PrestonsLaptop /cygdrive/d/academics/CS-403/projects/lisp-interpreter-cpp
$ make test
./tests/run
[doctest] doctest version is "2.4.11"
[doctest] run with "--help" for options
===============================================================================
[doctest] test cases:  48 |  48 passed | 0 failed | 0 skipped
[doctest] assertions: 246 | 246 passed | 0 failed |
[doctest] Status: SUCCESS!
