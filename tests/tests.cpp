#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#define DOCTEST_CONFIG_NO_POSIX_SIGNALS
#include "doctest.h"
#include "sexpression.h"

static shared_ptr<SExpression> parse(const string &str) {
    Reader reader(str);
    return reader.read();
}

TEST_SUITE("project 1.1") {
    TEST_CASE("1.1.1 data") {
        SUBCASE("1.1.1.1 makeAtom makes an atom") {
            auto a = makeAtom("x");
            CHECK(isAtom(a));
            CHECK(a->atomValue == "x");
        }

        SUBCASE("1.1.1.2 makeNil makes nil") {
            CHECK(isNil(makeNil()));
        }

        SUBCASE("1.1.1.3 makePair makes a pair") {
            auto p = makePair(makeAtom("a"), makeNil());
            CHECK(isPair(p));
        }

        SUBCASE("1.1.1.4 types are mutually exclusive") {
            auto a = makeAtom("a");
            CHECK(isAtom(a));
            CHECK_FALSE(isNil(a));
            CHECK_FALSE(isPair(a));
        }
    }

    TEST_CASE("1.1.2 reader") {
        SUBCASE("1.1.2.1 simple atom") { 
            CHECK(exprToString(parse("a")) == "a"); 
        }

        SUBCASE("1.1.2.2 flat list") {
             CHECK(exprToString(parse("(a b c)")) == "(a b c)"); 
        }

        SUBCASE("1.1.2.3 nested lists") { 
            CHECK(exprToString(parse("(a (b c) d)")) == "(a (b c) d)"); 
        }

        SUBCASE("1.1.2.4 empty list") { 
            CHECK(exprToString(parse("()")) == "()"); 
        }

        SUBCASE("1.1.2.5 malformed input throws") {
            SUBCASE("1.1.2.5.1 unexpected )") { 
                CHECK_THROWS_AS(parse(")"), runtime_error); 
            }
            SUBCASE("1.1.2.5.2 unterminated expression") { 
                CHECK_THROWS_AS(parse("(a b"), runtime_error); 
            }
        }
    }

    TEST_CASE("1.1.3 printef") {
        SUBCASE("1.1.3.1 atom") { 
            CHECK(exprToString(makeAtom("hi")) == "hi"); 
        }

        SUBCASE("1.1.3.2 nil") { 
            CHECK(exprToString(makeNil()) == "()"); 
        }

        SUBCASE("1.1.3.3 flat list") { 
            CHECK(exprToString(parse("(a b c)")) == "(a b c)"); 
        }

        SUBCASE("1.1.3.4 nested list") {
            CHECK(exprToString(parse("(a (b) c)")) == "(a (b) c)"); 
            }
    }

    TEST_CASE("1.1.4 read/print round-trips (spec examples)") {
        SUBCASE("1.1.4.1 empty list") { 
            CHECK(exprToString(parse("()")) == "()");
        }

        SUBCASE("1.1.4.2 atoms") {
            CHECK(exprToString(parse("a")) == "a");
            CHECK(exprToString(parse("45")) == "45");
            CHECK(exprToString(parse("test")) == "test");
        }
        SUBCASE("1.1.4.3 flat number list") { 
            CHECK(exprToString(parse("(1 2 3)")) == "(1 2 3)"); 
        }

        SUBCASE("1.1.4.4 list of lists") { 
            CHECK(exprToString(parse("((1 2 3) (a b c))")) == "((1 2 3) (a b c))"); 
        }

        SUBCASE("1.1.4.5 nested empty lists") { 
            CHECK(exprToString(parse("( ( ) ( ) ( ) )")) == "(() () ())"); 
        }
        SUBCASE("1.1.4.6 the big example") {
            CHECK(exprToString(parse("(() a 45 test (1 2 3) ((1 2 3) (a b c) ) ( ( ) ( ) ( ) ))")) == "(() a 45 test (1 2 3) ((1 2 3) (a b c)) (() () ()))");
        }
    }
}

static string run(const string &str) {
    Reader reader(str);
    return exprToString(eval(reader.read()));
}



TEST_SUITE("project 1.2") {

    TEST_CASE("1.2.1 accessors") {
        SUBCASE("1.2.11 car returns car") {
            CHECK(exprToString(car(makePair(makeAtom("a"), makeAtom("b")))) == "a");
        }

        SUBCASE("1.2.1.2 cdr returns cdr") {
            CHECK(exprToString(cdr(makePair(makeAtom("a"), makeAtom("b")))) == "b");
        }

        SUBCASE("1.2.1.3 cons onto nil is a 1-list") {
            CHECK(exprToString(cons(makeAtom("a"), makeNil())) == "(a)");
        }

        SUBCASE("1.2.1.4 cons of two atoms is dotted") {
            CHECK(exprToString(cons(makeAtom("a"), makeAtom("b"))) == "(a . b)");
        }
    }

    TEST_CASE("1.2.2 quote") {
        SUBCASE("1.2.2.1 quote of atom") {
            CHECK(run("(quote x)") == "x");
        }

        SUBCASE("1.2.2.2 quote does not evaluate") {
            CHECK(run("(quote (car x))") == "(car x)");
        }
    }

    TEST_CASE("1.2.3 eval") {
        SUBCASE("1.2.3.1 atom self-eval") {
            CHECK(run("a") == "a");
        }

        SUBCASE("1.2.3.2 nil selfeval") {
            CHECK(run("()") == "()");
        }

        SUBCASE("1.2.3.3 car") {
            CHECK(run("(car (quote (a b c)))") == "a");
        }

        SUBCASE("1.2.3.4 cdr") {
            CHECK(run("(cdr (quote (a b c)))") == "(b c)");
        }

        SUBCASE("1.2.3.5 cons dotted") {
            CHECK(run("(cons a b)") == "(a . b)");
        }

        SUBCASE("1.2.3.6 cons into nil") {
            CHECK(run("(cons a ())") == "(a)");
        }

        SUBCASE("1.2.3.7 cons builds a list") {
            CHECK(run("(cons a (cons b ()))") == "(a b)");
        }

        SUBCASE("1.2.3.8 eval of car") {
            CHECK(run("(eval (car (quote (a b c))))") == "a");
        }

        SUBCASE("1.2.3.9 eval of cdr recurses") {
            CHECK(run("(eval (cdr (quote (a b c))))") == "(b c)");
        }

        SUBCASE("1.2.3.10 eval rns quoted code") {
            CHECK(run("(eval (quote (car (quote (a b c)))))") == "a");
        }

        SUBCASE("1.2.3.11 unknown operator returns data") {
            CHECK(run("(bogus 'a)") == "(bogus (quote a))");
        }

        SUBCASE("1.2.3.12 non-atom head returns data") {
            CHECK(run("((quote a) 'b)") == "((quote a) (quote b))");
        }
    }

    TEST_CASE("1.2.4 reader quote shorthand") {
        SUBCASE("1.2.4.1 'a expands") {
            CHECK(exprToString(parse("'a")) == "(quote a)");
        }

        SUBCASE("1.2.4.2 '(a b) expands") {
            CHECK(exprToString(parse("'(a b)")) == "(quote (a b))");
        }

        SUBCASE("1.2.4.3 quote inside a list") {
            CHECK(exprToString(parse("(a 'b c)")) == "(a (quote b) c)");
        }

        SUBCASE("1.2.4.4 shorthand == long form") {
            CHECK(run("(car '(a b c))") == run("(car (quote (a b c)))"));
        }
    }
}