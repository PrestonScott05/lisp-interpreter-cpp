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

//1.6 helpers
//build a fram((names) (values)) from two src strings
static shared_ptr<SExpression> frame(const string &names, const string &values) {
    return list(parse(names), parse(values));
}

static string localSession(shared_ptr<SExpression> env, const vector<string> &exprs) {
    struct Reset { 
            ~Reset() {
                localEnv = makeNil(); 
        } 
    } reset;
    rho = makeNil();
    localEnv = env;
    string last;
    for (const auto &e : exprs) {
        Reader r(e);
        last = exprToString(eval(r.read()));
    }
    return last;
}

//for 1.6's spec
static shared_ptr<SExpression> requirementEnv() {
    return cons(frame("(a b c)", "(2 3 x)"), makeNil());
}

TEST_SUITE("Project 1.6") {
    TEST_CASE("1.6.1 local lookup") {
        SUBCASE("1.6.1.1 requirement example") {
            CHECK(localSession(requirementEnv(), {"a"}) == "2");
            CHECK(localSession(requirementEnv(), {"b"}) == "3");
            CHECK(localSession(requirementEnv(), {"c"}) == "x");
        }
        SUBCASE("1.6.1.2 locals work inside expressions") {
            CHECK(localSession(requirementEnv(), {"(add a b)"}) == "5");
            CHECK(localSession(requirementEnv(), {"(lt a b)"}) == "T");
        }
        SUBCASE("1.6.1.3 quoted symbols are not looked up") {
            CHECK(localSession(requirementEnv(), {"'b"}) == "b");
        }
        SUBCASE("1.6.1.4 unbound symbol returns itself") {
            CHECK(localSession(requirementEnv(), {"q"}) == "q");
        }
    }

    TEST_CASE("1.6.2 local vs global") {
        SUBCASE("1.6.2.1 local shadows global") {
            CHECK(localSession(requirementEnv(), {"(set a 99)", "a"}) == "2");
        }
        SUBCASE("1.6.2.2 falls through to global") {
            CHECK(localSession(requirementEnv(), {"(set z 5)", "z"}) == "5");
            CHECK(localSession(requirementEnv(), {"(set z 5)", "(add z b)"}) == "8");
        }
        SUBCASE("1.6.2.3 empty local stack behaves like before") {
            CHECK(localSession(makeNil(), {"b"}) == "b");
            CHECK(localSession(makeNil(), {"(set b 7)", "b"}) == "7");
        }
    }

    TEST_CASE("1.6.3 stack behavior") {
        SUBCASE("1.6.3.2 top frame always firt") {
            auto env = cons(frame("(a)", "(top)"), cons(frame("(a)", "(old)"), makeNil()));
            CHECK(localSession(env, {"a"}) == "top");
        }
        SUBCASE("1.6.3.2 outer frames are searched") {
            auto env = cons(frame("(b)", "(1)"), cons(frame("(a)", "(2)"), makeNil()));
            CHECK(localSession(env, {"a"}) == "2");
        }
        SUBCASE("1.6.3.3 popping via cdr restores outer scope") {
            auto env = cons(frame("(a)", "(inner)"), cons(frame("(a)", "(outer)"), makeNil()));
            CHECK(localSession(cdr(env), {"a"}) == "outer");
        }
        SUBCASE("1.6.3.4 empty frame falls through") {
            auto env = cons(frame("()", "()"), makeNil());
            CHECK(localSession(env, {"(set a 4)", "a"}) == "4");
        }
        SUBCASE("1.6.3.5 mismatched lengths don't crash") {
            auto env = cons(frame("(a b)", "(1)"), makeNil());
            CHECK(localSession(env, {"a"}) == "1");
            CHECK(localSession(env, {"b"}) == "b");
        }
    }

    TEST_CASE("1.6.4 function expressions") {
        SUBCASE("1.6.4.1 function evaluates to itself") {
            CHECK(run("(function (n) (add n 1))") == "(function (n) (add n 1))");
        }
        SUBCASE("1.6.4.2 empty arglist") {
            CHECK(run("(function () 5)") == "(function () 5)");
        }
        SUBCASE("1.6.4.3 set bins funtion") {
            CHECK(testSession({"(set foo (function (n) (add n 1)))", "foo"})
                  == "(function (n) (add n 1))");
        }
        SUBCASE("1.6.4.4 body not evaled at definition") {
            CHECK(localSession(requirementEnv(), {"(set bar (function () b))", "bar"})
                  == "(function () b)");
        }
    }
}

static string callSession(const vector<string> &exprs) {
    localEnv = makeNil();
    return testSession(exprs);
}

TEST_SUITE("Project 1.7") {
    TEST_CASE("1.7.1 basic  func calls") {
        SUBCASE("1.7.1.1 identity") {
            CHECK(callSession({"(set f (function (x) x))", "(f 5)"}) == "5");
        }
        SUBCASE("1.7.1.2 two params") {
            CHECK(callSession({"(set g (function (a b) (add a b)))", "(g 1 2)"}) == "3");
        }
        SUBCASE("1.7.1.3 zero params") {
            CHECK(callSession({"(set five (function () 5))", "(five)"}) == "5");
        }
        SUBCASE("1.7.1.4 actuals are evaluated") {
            CHECK(callSession({"(set g (function (a b) (add a b)))", "(g (add 1 2) 4)"}) == "7");
        }
        SUBCASE("1.7.1.5 quoted actual stays data") {
            CHECK( callSession({"(set f (function (x) (car x)))", "(f '(a b))"}) == "a");
        }
    }

    TEST_CASE("1.7.2 local/global scoping") {
        SUBCASE("1.7.2.1 param shadows global") {
            CHECK(callSession({"(set x 100)", "(set f (function (x) x))", "(f 1)"}) == "1");
        }
        SUBCASE("1.7.2.2 check if frame popped") {
            CHECK(callSession({"(set x 100)", "(set f (function (x) x))", "(f 1)", "x"}) == "100");
        }
        SUBCASE("1.7.2.3 body doesnt ignore globals") {
            CHECK(callSession({"(set k 10)", "(set f (function (n) (add n k)))", "(f 1)"}) == "11");
        }
        SUBCASE("1.7.2.4 actuals evaluated in CALLER frames") {
            CHECK(callSession({"(set f (function (x) x))",
                               "(set g (function (x y) (f y)))",
                               "(g 1 2)"}) == "2");
        }
        SUBCASE("1.7.2.5 callee  func can't see callers locals (no dynamic scope)") {
            CHECK(callSession({"(set h (function () y))",
                               "(set k (function (y) (h)))",
                               "(k 5)"}) == "y");
        }
    }

    TEST_CASE("1.7.3 recursion") {
        SUBCASE("1.7.3.1 factorial") {
            CHECK(callSession({"(set fact (function (n) (if (lt n 1) 1 (mul n (fact (sub n 1))))))",
                               "(fact 5)"}) == "120");
        }
        SUBCASE("1.7.3.2 list length") {
            CHECK(callSession({"(set len (function (l) (if (nil? l) 0 (add 1 (len (cdr l))))))",
                               "(len '(a b c))"}) == "3");
        }
        SUBCASE("1.7.3.3 mutual recursion") {
            CHECK(callSession({"(set ev (function (n) (if (eq? n 0) 'T (od (sub n 1)))))",
                               "(set od (function (n) (if (eq? n 0) () (ev (sub n 1)))))",
                               "(ev 4)"}) == "T");
        }
    }

    TEST_CASE("1.7.4 stack cleanlinesss") {
        SUBCASE("1.7.4.1 localEnv empty after a deep recursion func") {
            callSession({"(set fact (function (n) (if (lt n 1) 1 (mul n (fact (sub n 1))))))",
                         "(fact 5)"});
            CHECK(isNil(localEnv));
        }
        SUBCASE("1.7.4.2 localEnv restored when the body throws") {
            CHECK_THROWS_AS(callSession({"(set bad (function (n) (div n 0)))", "(bad 5)"}),
                            runtime_error);
            CHECK(isNil(localEnv));
            localEnv = makeNil();
        }
    }

    TEST_CASE("1.7.5 design decisions (documenting current behavior)") {
        SUBCASE("1.7.5.1 built-ins can't be shadowed") {
            CHECK(callSession({"(set add (function (a b) 0))", "(add 1 2)"}) == "3");
        }
        SUBCASE("1.7.5.2 unknown non-function head still returns data") {
            CHECK(callSession({"(bogus 1)"}) == "(bogus 1)");
        }
    }
}

TEST_SUITE("Project 1.8") {
    TEST_CASE("1.8.1 def basic tests") {
        SUBCASE("1.8.1.1 def returns the function form before eval") {
            CHECK(callSession({"(def add2 (a b) (add a b))"}) == "(function (a b) (add a b))");
        }
        SUBCASE("1.8.1.2 def binds the name") {
            CHECK(callSession({"(def add2 (a b) (add a b))", "add2"}) == "(function (a b) (add a b))");
        }
        SUBCASE("1.8.1.3 defined function is callable") {
            CHECK(callSession({"(def add2 (a b) (add a b))", "(add2 3 4)"}) == "7");
        }
        SUBCASE("1.8.1.4 zero params") {
            CHECK(callSession({"(def five () 5)", "(five)"}) == "5");
        }
    }

    TEST_CASE("1.8.2 def is sugar for set + function") {
        SUBCASE("1.8.2.1 same result as long form") {
            CHECK(callSession({"(def f (x) (add x 1))"}) == callSession({"(set f (function (x) (add x 1)))"}));
        }
        SUBCASE("1.8.2.2 we get same binding as long form") {
            CHECK(callSession({"(def f (x) (add x 1))", "(f 1)"}) == callSession({"(set f (function (x) (add x 1)))", "(f 1)"}));
        }
        SUBCASE("1.8.2.3 body not evaluated at definition") {
            CHECK(localSession(requirementEnv(), {"(def bar () b)", "bar"}) == "(function () b)");
        }
    }

    TEST_CASE("1.8.5 design decisions (documenting current behavior)") {
        SUBCASE("1.8.5.2 def inside a body defines globally") {
            CHECK(callSession({"(def mk () (def inner () 7))", "(mk)", "(inner)"}) == "7");
        }
        SUBCASE("1.8.5.3 too few args throws (unbound param falls to symbol)") {
            CHECK_THROWS_AS(callSession({"(def add2 (a b) (add a b))", "(add2 1)"}),
                            runtime_error);
            localEnv = makeNil();
        }
        SUBCASE("1.8.5.4 localEnv clean after def'd recursion") {
            callSession({"(def fact (n) (if (lt n 1) 1 (mul n (fact (sub n 1)))))", "(fact 5)"});
            CHECK(isNil(localEnv));
        }
    }
}


TEST_SUITE("final project extensions") {
    TEST_CASE("F.1 lambda and closures") {
        SUBCASE("F.1.1 lambda goes to a closure") {
            CHECK(testSession({"(lambda (x) x)"}) == "#<closure>");
        }
        SUBCASE("F.1.2 immediate application") {
            CHECK(testSession({"((lambda (x y) (add x y)) 3 4)"}) == "7");
        }
        SUBCASE("F.1.3 named lambda from set") {
            CHECK(testSession({"(set inc (lambda (x) (add x 1)))", "(inc 41)"}) == "42");
        }
        SUBCASE("F.1.4 closure gets its defining env") {
            CHECK(testSession({"(set adder (lambda (n) (lambda (x) (add x n))))",
                               "(set add5 (adder 5))", "(add5 10)"}) == "15");
        }
        SUBCASE("F.1.5 recursion through a global closure") {
            CHECK(testSession({"(set fact (lambda (n) (if (lt n 1) 1 (mul n (fact (sub n 1))))))",
                               "(fact 5)"}) == "120");
        }
    }

    TEST_CASE("F.2 begin") {
        SUBCASE("F.2.1 returns last value") {
            CHECK(testSession({"(begin 1 2 3)"}) == "3");
        }
        SUBCASE("F.2.2 runs effects in order") {
            CHECK(testSession({"(set x 1)", "(begin (set! x 2) (set! x (mul x 10)) x)"}) == "20");
        }
        SUBCASE("F.2.3 empty begin is nil") {
            CHECK(testSession({"(begin)"}) == "()");
        }
    }

    TEST_CASE("F.3 let / let*") {
        SUBCASE("F.3.1 let sets locals") {
            CHECK(testSession({"(let ((a 2) (b 3)) (mul a b))"}) == "6");
        }
        SUBCASE("F.3.2 let does not leak") {
            CHECK(testSession({"(let ((zz 1)) zz)", "zz"}) == "zz");
        }
        SUBCASE("F.3.3 let bindings can be parallel") {
            CHECK(testSession({"(set a 10)", "(let ((a 1) (b a)) b)"}) == "10");
        }
        SUBCASE("F.3.4 let* bindings can be sequential") {
            CHECK(testSession({"(let* ((a 1) (b (add a 1))) b)"}) == "2");
        }
        SUBCASE("F.3.5 let body has more forms") {
            CHECK(testSession({"(let ((a 1)) (set! a 5) a)"}) == "5");
        }
    }

    TEST_CASE("F.4 set! and mut closures") {
        SUBCASE("F.4.1 counter keep private state") {
            CHECK(testSession({
                "(set make-counter (lambda () (let ((n 0)) (lambda () (set! n (add n 1)) n))))",
                "(set c (make-counter))", "(c)", "(c)", "(c)"}) == "3");
        }
        SUBCASE("F.4.2 counters are indipendent") {
            CHECK(testSession({
                "(set make-counter (lambda () (let ((n 0)) (lambda () (set! n (add n 1)) n))))",
                "(set c1 (make-counter))", "(set c2 (make-counter))",
                "(c1)", "(c1)", "(c2)"}) == "1");
        }
        SUBCASE("F.4.3 set! on unbound variable throws") {
            CHECK_THROWS_AS(testSession({"(set! nope 1)"}), runtime_error);
            localEnv = makeNil();
        }
    }

    TEST_CASE("F.5 while") {
        SUBCASE("F.5.1 sums 1..10") {
            CHECK(testSession({"(set i 1)", "(set sum 0)",
                               "(while (lt i 11) (set! sum (add sum i)) (set! i (add i 1)))",
                               "sum"}) == "55");
        }
        SUBCASE("F.5.2 while returns nil") {
            CHECK(testSession({"(while ())"}) == "()");
        }
    }

    TEST_CASE("F.6 scoping edge cases") {
        SUBCASE("F.6.1 inner param shadows outer") {
            CHECK(testSession({"(let ((x 1)) (let ((x 2)) x))"}) == "2");
        }
        SUBCASE("F.6.2 outer binding visible after inner let ends") {
            CHECK(testSession({"(let ((x 1)) (let ((x 2)) x) x)"}) == "1");
        }
        SUBCASE("F.6.3 set! hits nearest binding only") {
            CHECK(testSession({"(let ((x 1)) (let ((x 2)) (set! x 99)) x)"}) == "1");
        }
        SUBCASE("F.6.4 set! on a global from inside a closure") {
            CHECK(testSession({"(set g 0)", "((lambda () (set! g 7)))", "g"}) == "7");
        }
        SUBCASE("F.6.5 lambda inside a def'd function captures its param") {
            CHECK(testSession({"(def mk (n) (lambda () n))", "(set f (mk 9))", "(f)"}) == "9");
        }
        SUBCASE("F.6.6 closure passed as an argument") {
            CHECK(testSession({"(set twice (lambda (f x) (f (f x))))",
                               "(twice (lambda (y) (mul y 3)) 2)"}) == "18");
        }
        SUBCASE("F.6.7 empty let") {
            CHECK(testSession({"(let () 4)"}) == "4");
        }
        SUBCASE("F.6.8 empty let*") {
            CHECK(testSession({"(let* () 4)"}) == "4");
        }
    }

    TEST_CASE("F.7 tail position does not grow the C++ stack") {
        SUBCASE("F.7.1 100k-deep tail recursion through a closure") {
            CHECK(testSession({"(set loop (lambda (n) (if (lt n 1) 'done (loop (sub n 1)))))",
                               "(loop 100000)"}) == "done");
        }
        SUBCASE("F.7.2 tail call through begin and let") {
            CHECK(testSession({"(set loop (lambda (n) (begin 0 (let ((m (sub n 1))) (if (lt m 0) 'ok (loop m))))))",
                               "(loop 50000)"}) == "ok");
        }
    }

    TEST_CASE("F.8 old behavior is preserved") {
        SUBCASE("F.8.1 def'd functions don't use dynamic scope") {
            CHECK(testSession({"(def f (x) (g))", "(def g () x)", "(f 5)"}) == "x");
        }
        SUBCASE("F.8.2 localEnv clea after closure calls") {
            testSession({"(set f (lambda (x) (add x 1)))", "(f 1)"});
            CHECK(isNil(localEnv));
        }
    }
}  


static string captureOutput(const vector<string> &expressions) {
    ostringstream captured;
    streambuf *old = cout.rdbuf(captured.rdbuf());
    try {
        testSession(expressions);
    } catch (...) {
        cout.rdbuf(old);
        throw;
    }
    cout.rdbuf(old);
    return captured.str();
}


static string withInput(const string &input, const vector<string> &expressions) {
    istringstream fake(input);
    streambuf *old = cin.rdbuf(fake.rdbuf());
    string result = testSession(expressions);
    cin.rdbuf(old);
    return result;
}

TEST_SUITE("final project batch 2: strings + io") {
    TEST_CASE("S.1 string literals") {
        SUBCASE("S.1.1 reader reads a string") {
            CHECK(exprToString(parse("\"hello\"")) == "\"hello\"");
        }
        SUBCASE("S.1.2 strings evaluate to themselves") {
            CHECK(testSession({"\"hi there\""}) == "\"hi there\"");
        }
        SUBCASE("S.1.3 parens and quotes inside a string are just text") {
            CHECK(testSession({"\"(not a list)\""}) == "\"(not a list)\"");
        }
        SUBCASE("S.1.4 escapes round-trip") {
            CHECK(testSession({"\"a\\\"b\\\\c\\nd\""}) == "\"a\\\"b\\\\c\\nd\"");
        }
        SUBCASE("S.1.5 string inside a list") {
            CHECK(testSession({"'(1 \"two\" 3)"}) == "(1 \"two\" 3)");
        }
        SUBCASE("S.1.6 unterminated string throws") {
            CHECK_THROWS_AS(parse("\"oops"), runtime_error);
        }
        SUBCASE("S.1.7 string bound toa variable") {
            CHECK(testSession({"(set name \"Preston\")", "name"}) == "\"Preston\"");
        }
    }

    TEST_CASE("S.2 string builtins") {
        SUBCASE("S.2.1 string? true for strings") {
            CHECK(testSession({"(string? \"x\")"}) == "T");
        }
        SUBCASE("S.2.2 string? false for symbols and numbers") {
            CHECK(testSession({"(string? 'x)"}) == "()");
            CHECK(testSession({"(string? 5)"}) == "()");
        }
        SUBCASE("S.2.3 string-append joins any number of strings") {
            CHECK(testSession({"(string-append \"a\" \"b\" \"c\")"}) == "\"abc\"");
        }
        SUBCASE("S.2.4 string-append with no args") {
            CHECK(testSession({"(string-append)"}) == "\"\"");
        }
        SUBCASE("S.2.5 string-append rejects non-strings") {
            CHECK_THROWS_AS(testSession({"(string-append \"a\" 5)"}), runtime_error);
        }
        SUBCASE("S.2.6 string-length") {
            CHECK(testSession({"(string-length \"hello\")"}) == "5");
            CHECK(testSession({"(string-length \"\")"}) == "0");
        }
        SUBCASE("S.2.7 eq? compares string contents") {
            CHECK(testSession({"(eq? \"ab\" (string-append \"a\" \"b\"))"}) == "T");
            CHECK(testSession({"(eq? \"ab\" \"ba\")"}) == "()");
        }
        SUBCASE("S.2.8 a string is not the symbol with the same name") {
            CHECK(testSession({"(eq? \"x\" 'x)"}) == "()");
        }
    }

    TEST_CASE("S.3 display / newline") {
        SUBCASE("S.3.1 display prints strings without quotes") {
            CHECK(captureOutput({"(display \"hi\")"}) == "hi");
        }
        SUBCASE("S.3.2 display prints other values normally") {
            CHECK(captureOutput({"(display '(1 2))"}) == "(1 2)");
            CHECK(captureOutput({"(display (add 2 3))"}) == "5");
        }
        SUBCASE("S.3.3 display takes multiple args") {
            CHECK(captureOutput({"(display \"x = \" 42)"}) == "x = 42");
        }
        SUBCASE("S.3.4 newline prints a newline") {
            CHECK(captureOutput({"(newline)"}) == "\n");
        }
        SUBCASE("S.3.5 display and newline return void") {
            ostringstream sink;

            streambuf *old = cout.rdbuf(sink.rdbuf());
            
            string displayResult = testSession({"(display \"x\")"});
            string newlineResult = testSession({"(newline)"});
            
            cout.rdbuf(old);
            
            CHECK(displayResult == "");
            CHECK(newlineResult == "");
        }
        SUBCASE("S.3.6 printing inside a loop") {
            CHECK(captureOutput({"(set i 0)",
                                 "(while (lt i 3) (display i) (set! i (add i 1)))"}) == "012");
        }
        SUBCASE("S.3.7 printing from a closure") {
            CHECK(captureOutput({"(set greet (lambda (who) (display \"hi \" who) (newline)))",
                                 "(greet \"Preston\")"}) == "hi Preston\n");
        }
    }

    TEST_CASE("S.4 read-line") {
        SUBCASE("S.4.1 reads one line as a string") {
            CHECK(withInput("hello world\n", {"(read-line)"}) == "\"hello world\"");
        }
        SUBCASE("S.4.2 successive calls read successive lines") {
            CHECK(withInput("first\nsecond\n", {"(read-line)", "(read-line)"}) == "\"second\"");
        }
        SUBCASE("S.4.3 returns nil at EOF") {
            CHECK(withInput("", {"(read-line)"}) == "()");
        }
        SUBCASE("S.4.4 result works with string builtins") {
            CHECK(withInput("abc\n", {"(string-length (read-line))"}) == "3");
        }
    }

    TEST_CASE("S.5 load") {
        const string path = "tests/load_fixture.lisp";
        {
            ofstream out(path);
            out << "; a library file\n"
                << "(set square (lambda (x) (mul x x)))\n"
                << "(set greeting \"loaded (with parens)\")\n";
        }

        SUBCASE("S.5.1 load defines things for later expressions") {
            CHECK(testSession({"(load \"tests/load_fixture.lisp\")", "(square 7)"}) == "49");
        }
        SUBCASE("S.5.2 strings in loaded files work") {
            CHECK(testSession({"(load \"tests/load_fixture.lisp\")", "greeting"}) == "\"loaded (with parens)\"");
        }
        SUBCASE("S.5.3 load returns void") {
            CHECK(testSession({"(load \"tests/load_fixture.lisp\")"}) == "");
        }
        SUBCASE("S.5.4 missing file throws") {
            CHECK_THROWS_AS(testSession({"(load \"tests/does_not_exist.lisp\")"}), runtime_error);
        }
        SUBCASE("S.5.5 non-string path throws") {
            CHECK_THROWS_AS(testSession({"(load 5)"}), runtime_error);
        }

        remove(path.c_str());
    }
}
