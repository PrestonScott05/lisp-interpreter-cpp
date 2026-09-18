#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <memory>
#include <cctype>
#include <stdexcept>
#include <sstream>

using namespace std;



enum class ExpressionType {
    Nil, 
    Atom, 
    Pair
};

enum class Operator {
    Quote, 
    Car, 
    Cdr, 
    Cons, 
    Eval,
    Set,

    //predicates (?suffixed)
    Nil_q, Atom_q, List_q, Number_q,
    Unknown
};

struct SExpression {
    ExpressionType type;
    string atomValue;


    shared_ptr<SExpression> car;
    shared_ptr<SExpression> cdr;
};

inline shared_ptr<SExpression> makeAtom(string value) {
    shared_ptr<SExpression> atom = make_shared<SExpression>();
    
    atom->type = ExpressionType::Atom;
    atom->atomValue = move(value);
    atom->car = nullptr;
    atom->cdr = nullptr;

    return atom;
}

inline shared_ptr<SExpression> makeNil() {
    shared_ptr<SExpression> nil = make_shared<SExpression>();

    nil->type = ExpressionType::Nil;
    nil->car = nullptr;
    nil->cdr = nullptr;

    return nil;
}

inline shared_ptr<SExpression> makePair(shared_ptr<SExpression> myCar, shared_ptr<SExpression> myCdr) {
    shared_ptr<SExpression> pair = make_shared<SExpression>();

    pair->type = ExpressionType::Pair;
    pair->car = myCar;
    pair->cdr = myCdr;

    return pair;
}

inline shared_ptr<SExpression> rho = makeNil();
const inline shared_ptr<SExpression> TRUE_ = makeAtom("T");
const inline shared_ptr<SExpression> FALSE_ = makeNil();

inline bool isNil(shared_ptr<SExpression> node) {
    return node->type == ExpressionType::Nil;
}

inline bool isAtom(shared_ptr<SExpression> node) {
    return node->type == ExpressionType::Atom;
}

inline bool isPair(shared_ptr<SExpression> node) {
    return node->type == ExpressionType::Pair;
}

inline bool isInteger(const string &s) {
    if (s.empty()) return false;

    size_t start = 0;
    if (s[0] == '-') {
        if (s.length() == 1) {
            return false;
        }
        start = 1;
    }

    for (size_t i = start; i < s.length(); i++) {
        if (!isdigit(static_cast<unsigned char>(s[i]))) {
            return false;
        }
    }

    return true;
}

inline shared_ptr<SExpression> car(shared_ptr<SExpression> cell) {
    return cell->car;
}

inline shared_ptr<SExpression> cdr(shared_ptr<SExpression> cell) {
    return cell->cdr;
}

inline shared_ptr<SExpression> cons(shared_ptr<SExpression> a, shared_ptr<SExpression> b) {
    return makePair(a, b);
}

class Reader {
public: 
    explicit Reader(string str) : sourceString(move(str)), pos(0) {}

    shared_ptr<SExpression> read() {
        skipWhitespace();
        
        if (atEnd()) {
            throw runtime_error("we hit the end of the input unexpectedly");
        }

        char curr = peek();

        if (curr == ')') {
            throw runtime_error("unexpected ')' with no matching '('");
        }

        if (curr == '\'') {
            advance();
            shared_ptr<SExpression> quoted = read();
            shared_ptr<SExpression> result = makePair(makeAtom("quote"), makePair(quoted, makeNil()));

            return result;
        }
        
        if (curr == '(') {
            return readList();
        } else {
            return readAtom();
        }
    }

    bool hasMoreStuff() {
        skipWhitespace();
        return !atEnd();
    }

private:
    string sourceString;
    size_t pos = 0;

    void skipWhitespace() {
        while (!atEnd()) {
            char current = peek();

            if (isspace(static_cast<unsigned char>(current)) || current == ',') {
                advance();
            } else {
                break;
            }
        }
    }

    shared_ptr<SExpression> readAtom() {
        string result = "";

        while (!atEnd()) {
            char c = peek();
            if (c == ' ' || c == '\n' || c == '\t' || c == '\r' || c == '(' || c == ')' || c == ',' || c == '\'') {
                break;
            }

            result += advance();
        }

        return makeAtom(result);
    }

    shared_ptr<SExpression> readList() {
        advance();

        vector<shared_ptr<SExpression>> elements;

        while (true) {
            skipWhitespace();

            if (atEnd()) {
                throw runtime_error("you didn't terminate the list. missing ')'");
            }

            if (peek() == ')') {
                advance();
                break;
            }

            elements.push_back(read());
        }

        shared_ptr<SExpression> list = makeNil();

        for (auto iterator = elements.rbegin(); iterator != elements.rend(); iterator++) {
            list = makePair(*iterator, list);
        }

        return list;
    }

    char peek() {
        if (atEnd()) {
            return '\0';
        } else {
            return sourceString[pos];
        }
    }

    char advance() {
        char c = sourceString[pos];
        pos++;
        return c;
    }

    bool atEnd() const {
        if (pos >= sourceString.size()) {
            return true;
        } else {
            return false;
        }
    }
};

inline string exprToString(const shared_ptr<SExpression> &expr) {
    if (isAtom(expr)) return expr->atomValue;
    if (isNil(expr)) return "()";

    string out = "(";

    auto cur = expr;
    bool first = true;

    while (isPair(cur)) {
        if (!first) {
            out += " ";
        }

        out += exprToString(cur->car);
        cur = cur->cdr;
        first = false;
    }

    if (!isNil(cur)) {
        out += " . " + exprToString(cur);
    }

    return out + ")";
}

inline void print(const shared_ptr<SExpression> &expr) {
    cout << exprToString(expr);
}

inline shared_ptr<SExpression> list(shared_ptr<SExpression> a, shared_ptr<SExpression> b) {
    return cons(a, cons(b, makeNil()));
}

inline shared_ptr<SExpression> lookup(shared_ptr<SExpression> symbol, shared_ptr<SExpression> environment) {
    while (isPair(environment)) {
        shared_ptr<SExpression> entry = car(environment);
        shared_ptr<SExpression> name = car(entry);

        if (name->atomValue == symbol->atomValue) return car(cdr(entry));
        environment = cdr(environment);
    }

    return symbol;
}

inline Operator toOp(const string &s) {
    if (s == "quote") return Operator::Quote;
    if (s == "car") return Operator::Car;
    if (s == "cdr") return Operator::Cdr;
    if (s == "cons") return Operator::Cons;
    if (s == "eval") return Operator::Eval;
    if (s == "set") return Operator::Set;
    if (s == "nil?") return Operator::Nil_q;
    if (s == "atom?") return Operator::Atom_q;
    if (s == "list?") return Operator::List_q;
    if (s == "not?") return Operator::Nil_q;
    if (s == "number?") return Operator::Number_q;
    return Operator::Unknown;
}

inline shared_ptr<SExpression> eval(shared_ptr<SExpression> expression) {
    if (isAtom(expression)) return lookup(expression, rho);
    if (isNil(expression)) return expression;

    shared_ptr<SExpression> operation = car(expression);

    if (!isAtom(operation)) {
        return expression;
    }

    switch(toOp(operation->atomValue)) {
        case Operator::Quote: {
            shared_ptr<SExpression> arg = car(cdr(expression));
            return arg;
        }
        case Operator::Car: {
            shared_ptr<SExpression> arg = car(cdr(expression));
            return car(eval(arg));
        }
        case Operator::Cdr: {
            shared_ptr<SExpression> arg = car(cdr(expression));
            return cdr(eval(arg));
        }
        case Operator::Cons: {
            shared_ptr<SExpression> a = car(cdr(expression));
            shared_ptr<SExpression> b = car(cdr(cdr(expression)));
            return cons(eval(a), eval(b));  
        }
        case Operator::Eval: {
            shared_ptr<SExpression> arg = car(cdr(expression));
            return eval(eval(arg));
        }
        case Operator::Set: {
            shared_ptr<SExpression> name = car(cdr(expression));
            shared_ptr<SExpression> value = car(cdr(cdr(expression)));

            shared_ptr<SExpression> value_result = eval(value);

            rho = cons(list(name, value_result), rho);
            return value_result;
        }
        case Operator::Nil_q: {
            shared_ptr<SExpression> arg = car(cdr(expression));

            if (isNil(eval(arg))) {
                return TRUE_;
            } else {
                return FALSE_;
            }
        }

        case Operator::Atom_q: {
            shared_ptr<SExpression> arg = car(cdr(expression));

            if (isAtom(eval(arg))) {
                return TRUE_;
            } else {
                return FALSE_;
            }
        }

        case Operator::List_q: {
            shared_ptr<SExpression> arg = car(cdr(expression));

            if (isPair(eval(arg))) {
                return TRUE_;
            } else {
                return FALSE_;
            }
        }

        case Operator::Number_q: {
            shared_ptr<SExpression> arg = car(cdr(expression));
            shared_ptr<SExpression> result = eval(arg);

            if (!isAtom(result)) {
                return FALSE_;
            }

            if (isInteger(result->atomValue)) {
                return TRUE_;
            } else {
                return FALSE_;
            }
        }
        case Operator::Unknown: {
            return expression;
        }
    }
    throw runtime_error("eval unreachable");
}