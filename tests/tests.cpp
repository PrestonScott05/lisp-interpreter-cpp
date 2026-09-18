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

    TEST_CASE("1.1.3 printer") {
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