# S-Expression reader

Programming Languages (CS-403), The University of Alabama.
Built by Preston Lumpkins.

this is a read-and-pring loop for S-expressions. this first part (1.1) reads S-expressions and prints them back in token form. There is no evalutation yet. 

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
=> (a (b c) d)
(a (b c) d)

---

End the session with Ctrl-D or pipe / redirect input, which prints each resultl with no prompt:

    echo '(1 2 3)' | ./repl
    ./repl < input.txt

---

Expressions may span multiple lines, and a line may hold several expressions. 

Malformed input prints an error and the loop continues. 

## What it does

all requirements up to project 1.3 - Global Variables
