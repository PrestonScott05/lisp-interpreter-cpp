# Exam 1 review
**not many questions** - ballpark 6. 4 < number of questions < 10
## grammar
given a grammar in one form, convert to another (without changing the described language);
- 2-3 CNF/GNF/BNF/EBNF

### GNF - IN CLASS

S' -> S $
S -> a A B | b B A | ε 
A -> a | S
b -> b | S

---
#### 1. remove epsilons
S' -> S $ | $
S -> a A B | a A | a B | a | b B A | b B | b A | b
A -> a | S
B -> b | S

#### 2. check for GNF (begins with terminal followed by any number of nonterminals, 1 terminal);
- here, S is the only rule fully in GNF, so substitute into the places where S is. 

S' -> a A B Z | a A Z | a B Z | a Z | b B A Z | b B Z | b A Z | b Z | $
S -> a A B | a  A | a B | b B A | b B | b A | a | b
A -> a A B | a  A | a B | b B A | b B | b A | a | b
B -> a A B | a  A | a B | b B A | b B | b A | a | b
Z -> $

---

### In-class # 2: given an EBNF, convert to  BNF
ebnf: {} -> zero or more times
ebnf: []  -> optional ()? 0 or 1

<expression> ::= <term> { ('+' | '-') <term>}
<term> :== <factor> { ('*' | '/') <factor> }
<factor> :== <number> | '(' <expression> ')'
<digit> :== '0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9' | '0'

---
<expression> ::= <term> <exprrest>
<exprrest> ::= | <plusop> <term> <exprrest>
<plusop> ::= '+' | '-'

<term> ::= <factor> <fact>
<fact> ::= | <mulop> <factor> <fact>
<mulop> ::= '*' | '/'

<factor> ::= <number> | '(' <expression> ')'
<number> ::= <digit> |  <number> <digit>

<digit> :== '0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9' | '0'

---

### In-class CNF practice:
- using the bnf from above: 
    <expression> ::= <term> <exprrest>  
    <exprrest> ::= | <plusop> <term> <exprrest>
    <plusop> ::= '+' | '-'

    <term> ::= <factor> <fact> |
    <fact> ::= | <mulop> <factor> <fact>
    <mulop> ::= '*' | '/'

    <factor> ::= <number> | '(' <expression> ')'
    <number> ::= <digit> |  <number> <digit>

    <digit> :== '0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9' | '0'

#### remove eps:
    <expression> ::= <term> <exprrest> | <term>
    <exprrest> ::= <plusop> <term> <exprrest> | <plussop> <term> <---
    <plusop> ::= '+' | '-'

    <term> ::= <factor> <fact> | <factor>
    <fact> ::= <mulop> <factor> <fact> | <mulop> <factor><---

    <mulop> ::= '*' | '/'

    <factor> ::= <number> | '(' <expression> ')'
    <number> ::= <digit> |  <number> <digit>

    <digit> :== '0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9' | '0'

#### 2. put in CNF (single terminal or two nonterminals -> maxlen = 2)
    <expression> ::= <term> <exprrest> | <factor> <fact> | '0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9' | '0' |  <number> <digit> | '(' <expression> ')'
    <exprrest> ::= <plusop> <term> <exprrest> | <plussop> <term> <---
    <plusop> ::= '+' | '-'

    <term> ::= <factor> <fact> | '0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9' | '0' |  <number> <digit> | '(' <expression> ')'
    <fact> ::= <mulop> <factor> <fact> | <mulop> <factor><---

    <mulop> ::= '*' | '/'

    <factor> ::= '0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9' | '0' |  <number> <digit> | '(' <expression> ')'
    <number> ::= '0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9' | '0' |  <number> <digit>

    <digit> :== '0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9' | '0'

#### 3. put in CNF (split rules > 2)
    <expression> ::= <term> <exprrest> | <factor> <fact> | '0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9' | '0' |  <number> <digit> | <LP> <E2> <---
    <exprrest> ::= <plusop> <exprrest2> | <plussop> <term> <---
    <plusop> ::= '+' | '-'
    <exprrest2> ::= <term> <exprrest>
    <term> ::= <factor> <fact> | '0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9' | '0' |  <number> <digit> | <LP> <E2> <---
    <fact> ::= <mulop> <fact2> | <mulop> <factor>

    <mulop> ::= '*' | '/'
    <fact2> ::= <factor> <fact>

    <factor> ::= '0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9' | '0' |  <number> <digit> | <LP> <E2> <---
    <number> ::= '0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9' | '0' |  <number> <digit>

    <digit> :== '0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9' | '0'
--> <LP> -> '('
--> <RP> -> ')'
--> <E2> -> <expression> <RP>
---
## lisp
- 2x LISP problems
- somewhat small -- easy to understand

## History 
- atm, one question

## Lox
parsing and lexical analysis

## Other forms:
- create a grammar to describe... (i.e. csv)