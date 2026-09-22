#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#define DOCTEST_CONFIG_NO_POSIX_SIGNALS
#include "doctest.h"
#include "sexpression.h"

static shared_ptr<SExpression> parse(const string &str) {
    Reader reader(str);
    return reader.read();
}


static string run(const string &str) {
    rho = makeNil(); 
    Reader reader(str);
    return exprToString(eval(reader.read()));
}

static string testSession(const vector<string> &expressions) {
    rho = makeNil();
    string last;
    for (const auto &e : expressions) {
        Reader r(e);
        last = exprToString(eval(r.read()));
    }
    return last;
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

    TEST_CASE("1.1.added_later error paths") {
        SUBCASE(" car of non-pair throws") {
            CHECK_THROWS_AS(run("(car 'a)"), runtime_error);
        }
        SUBCASE("cdr of non-pair throws") {
            CHECK_THROWS_AS(run("(cdr 'a)"), runtime_error);
        }
        SUBCASE("car of nil throws") {
            CHECK_THROWS_AS(run("(car ())"), runtime_error);
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


TEST_SUITE("Project 1.3") {
    TEST_CASE("1.3.1 set and lookup funs") {
        SUBCASE("1.3.1.1 set returns the bound val") {
            CHECK(run("(set a 2)") == "2");
        }

        SUBCASE("1.3.1.2 set then lookup") {
            CHECK(testSession({"(set a 2)", "a"}) == "2");
        }

        SUBCASE("1.3.1.3 lookup misses returns the symbol") {
            CHECK(run("x") == "x");
        }

        SUBCASE("1.3.1.4 newest set overrides") {
            CHECK(testSession({"(set a 2)", "(set a 9)", "a"}) == "9");
        }
    }

    TEST_CASE("1.3.2 predicates") {
        SUBCASE("1.3.2.1 nil?") {
            CHECK(run("(nil? ())") == "T");
            CHECK(run("(nil? 'T)") == "()");
        }

        SUBCASE("1.3.2.2 atom?") {
            CHECK(run("(atom? x)") == "T");
            CHECK(run("(atom? (a))") == "()");
            CHECK(run("(atom? ())") == "()");
            CHECK(run("(atom? 123)") == "T");
        }

        SUBCASE("1.3.2.3 list?") {
            CHECK(run("(list? 'T)") == "()");
            CHECK(run("(list? (x))") == "T");
            CHECK(run("(list? a)") == "()");
        }

        SUBCASE("1.3.2.4 number?") {
            CHECK(run("(number? 42)") == "T");
            CHECK(run("(number? -42)") == "T");

            CHECK(run("(number? 42.31)") == "()");
            CHECK(run("(number? -42.31)") == "()");

            CHECK(run("(number? x)") == "()");
            CHECK(run("(number? (a b))") == "()");

            CHECK(run("(number? -)") == "()");
            CHECK(run("(number? ())") == "()");
        }
    }
}

TEST_SUITE("Project 1.4") {
    TEST_CASE("1.4.1 and? tests") {
        SUBCASE("1.4.1.1 both non nil returns true") {
            CHECK(run("(and? 'a 'b)") == "T");
        }
        SUBCASE("1.4.1.2 first nil produces ()") {    
            CHECK(run("(and? () 'b)") == "()");
        }
        SUBCASE("1.4.1.3 second nil produces ()") {
            CHECK(run("(and? 'a ())") == "()");
        }

        SUBCASE("1.4.1.4 both nil yields ()") {
            CHECK(run("(and? () ())") == "()");
        }

        SUBCASE("1.4.1.5 short-circuits: b not evaluated when a is nil") {
            CHECK(run("(and? () (car ()))") == "()");
        }
    }

    TEST_CASE("1.4.2 or?") {
        SUBCASE("1.4.2.1 both nil yeilds ()") {
            CHECK(run("(or? () ())") == "()");
        }
        SUBCASE("1.4.2.2 first non-nil gives T") {
            CHECK(run("(or? 'a ())") == "T");
        }
        SUBCASE("1.4.2.3 second non-nil gives T") {
            CHECK(run("(or? () 'b)") == "T");
        }
        SUBCASE("1.4.2.4 both non-nil gives  T") {
            CHECK(run("(or? 'a 'b)") == "T");
        }
        SUBCASE("1.4.2.5 short-circuits: b not evaluated when a is non-nil") {
            // we use an invalid operation to confirm the short circuit because this would throw an error. 
            CHECK(run("(or? 'a (car ()))") == "T");
        }
    }

    TEST_CASE("1.4.3 eq?") {
        SUBCASE("1.4.3.1 same symbol makes T") {
            CHECK(run("(eq? 'a 'a)") == "T");
        }
        SUBCASE("1.4.3.2 different symbols makes ()") {
            CHECK(run("(eq? 'a 'b)") == "()");
        }
        SUBCASE("1.4.3.3 args are evaluated") {
            CHECK(testSession({"(set a 2)", "(set b 2)", "(eq? a b)"}) == "T");
        }
        SUBCASE("1.4.3.4 evaluated to different makes ()") {
            CHECK(testSession({"(set a 2)", "(set b 4)", "(eq? a b)"}) == "()");
        }
        SUBCASE("1.4.3.5 nil is not an atom makes ()") {
            CHECK(run("(eq? () ())") == "()");
        }
        SUBCASE("1.4.3.6 lists are not compared -> ()") {
            CHECK(run("(eq? '(a) '(a))") == "()");
        }
        SUBCASE("1.4.3.7 atom vs list -> ()") {
            CHECK(run("(eq? 'a '(a))") == "()");
        }
        SUBCASE("1.4.3.8 numbers compare by symbol and not as an atom") {
            CHECK(run("(eq? 42 42)") == "T");
            CHECK(run("(eq? 42 43)") == "()");
        }
    }

    TEST_CASE("1.4.4 if") {
        SUBCASE("1.4.4.1 true branch") {
            CHECK(run("(if 'T ''T ())") == "(quote T)");
        }
        SUBCASE("1.4.4.2 false branch") {
            CHECK(run("(if () ''T ())") == "()");
        }
        SUBCASE("1.4.4.3 untaken branch not evaluated (true)") {
            //else condition would throw
            CHECK(run("(if 'T 'ok (car ()))") == "ok");
        }
        SUBCASE("1.4.4.4 untaken branch not evaluated (false)") {
            //first condition would throw
            CHECK(run("(if () (car ()) 'ok)") == "ok");
        }
        SUBCASE("1.4.4.5 missing else, false cond -> ()") {
            CHECK(run("(if () 'then)") == "()");
        }
        SUBCASE("1.4.4.6 condition is evaluated") {
            CHECK(testSession({"(set c ())", "(if c 'yes 'no)"}) == "no");
        }
    }

    TEST_CASE("1.4.5 cond") {
        SUBCASE("1.4.5.1 first match wins") {
            CHECK(run("(cond ('T 'first 'T 'second))") == "first");
        }
        SUBCASE("1.4.5.2 skips nil clauses") {
            CHECK(run("(cond (() 'skip 'T 'taken))") == "taken");
        }
        SUBCASE("1.4.5.3 T default fires when all else nil (just a good practice)") {
            CHECK(run("(cond (() 'a () 'b 'T 'default))") == "default");
        }
        SUBCASE("1.4.5.4 only matched b is evaluated") {
            //again would throw, so we make sure this isn't evaled
            CHECK(run("(cond (() (car ()) 'T 'safe))") == "safe");
        }
        SUBCASE("1.4.5.5 no clause matches -> throws") {
            CHECK_THROWS_AS(run("(cond (() 'a () 'b))"), runtime_error);
        }
        SUBCASE("1.4.5.6 condition exprs are evaluated") {
            CHECK(testSession({"(set flag 'T)", "(cond (flag 'on 'T 'off))"}) == "on");
        }
    }
}

TEST_SUITE("Project 1.5") {
    TEST_CASE("1.5.1 add tests") {
        SUBCASE("1.5.1.1 basic") {
            CHECK(run("(add 2 3)") == "5");
        }
        SUBCASE("1.5.1.2 negatives") {
            CHECK(run("(add -2 3)") == "1");
            CHECK(run("(add -5 -4)") == "-9");
        }

        SUBCASE("1.5.1.3 args get evaled") {
            CHECK(testSession({"(set a 10)", "(add a 5)"}) == "15");
        }

        SUBCASE("1.5.1.4 nests") {
            CHECK(run("(add (add 1 2) 3)") == "6");
        }
    }

    TEST_CASE("1.5.2 sub") {

        SUBCASE("1.5.2.1 basic") {
            CHECK(run("(sub 7 3)") == "4");
        }

        SUBCASE("1.5.2.2 can get negative results") {
            CHECK(run("(sub 3 7)") == "-4");
        }
        SUBCASE("1.5.2.3 args get evaled") {
            CHECK(testSession({"(set a 5)", "(sub a 5)"}) == "0");
        }

    }

    TEST_CASE("1.5.3 mul") {
        SUBCASE("1.5.3.1 basic") {
            CHECK(run("(mul 4 5)") == "20");
        }
        SUBCASE("1.5.3.2 by zero") {
            CHECK(run("(mul 4 0)") == "0");
        }
        SUBCASE("1.5.3.3 sign rules apply") {
            CHECK(run("(mul -3 4)") == "-12");
        }

        SUBCASE("1.5.3.4 args get evaled") {
            CHECK(testSession({"(set a 10)", "(set b 5)", "(mul a b)"}) == "50");
        }
    }

    TEST_CASE("1.5.4 div") {
        SUBCASE("1.5.4.1 exact") {
            CHECK(run("(div 12 4)") == "3");
        }
        SUBCASE("1.5.4.2 integer division truncates toward zero") {
            CHECK(run("(div 7 2)") == "3");
        }
        SUBCASE("1.5.4.3 divide by zero throws") {
            CHECK_THROWS_AS(run("(div 5 0)"), runtime_error);
        }

        SUBCASE("1.5.4.4 arges get evaluated") {
            CHECK(testSession({"(set a 9)", "(set b 3)", "(div a b)"}) == "3");
        }
    }

    TEST_CASE("1.5.5 rem") {
        SUBCASE("1.5.5.1 basic") {
            CHECK(run("(rem 7 3)") == "1");
        }
        SUBCASE("1.5.5.2 exact -> 0") {
            CHECK(run("(rem 8 4)") == "0");
        }
        SUBCASE("1.5.5.3 rem by zero throws") {
            CHECK_THROWS_AS(run("(rem 5 0)"), runtime_error);
        }

        SUBCASE("1.5.5.4 arguments evaled") {
            CHECK(testSession({"(set a 12)", "(set b 5)", "(rem a b)"}) == "2");
        }
    }

    TEST_CASE("1.5.6 lt") {
        SUBCASE("1.5.6.1 less -> T") {
            CHECK(run("(lt 2 5)") == "T");
        }
        SUBCASE("1.5.6.2 greater -> ()") {
            CHECK(run("(lt 5 2)") == "()");
        }
        SUBCASE("1.5.6.3 equal -> ()") {
            CHECK(run("(lt 4 4)") == "()");
        }
        SUBCASE("1.5.6.4 negatives") {
            CHECK(run("(lt -3 0)") == "T");
        }
        SUBCASE("1.5.6.5 args are evaluated") {
            CHECK(testSession({"(set a 1)", "(set b 2)", "(lt a b)"}) == "T");
        }
    }

    TEST_CASE("1.5.7 type errors") {
        SUBCASE("1.5.7.1 non-number atom throws") {
            CHECK_THROWS_AS(run("(add 'x 5)"), runtime_error);
        }
        SUBCASE("1.5.7.2 nil is not a number") {
            CHECK_THROWS_AS(run("(add () 5)"), runtime_error);
        }
        SUBCASE("1.5.7.3 list is not a number") {
            CHECK_THROWS_AS(run("(lt '(a) 5)"), runtime_error);
        }
    }
}