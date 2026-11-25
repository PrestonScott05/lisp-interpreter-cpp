#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <memory>
#include <cctype>
#include <stdexcept>
#include <sstream>
#include <fstream>

using namespace std;



enum class ExpressionType {
    Nil, 
    Atom, 
    Pair,
    Closure, 
    String, 
    Void,
};

enum class Operator {
    Quote, 
    Car, 
    Cdr, 
    Cons, 
    Eval,
    Set, // currently only global

    //predicates (?suffixed)
    Nil_q, Atom_q, List_q, Number_q,

    //logic operators
    And_q, Or_q, Eq_q,

    //conditionality
    If, Cond,

    //math
    Add, Subtract, Multiply, Divide, Remainder, 

    //Relations
    LessThan,

    //functions!! (finally)
    Function, 
    Def,

    Lambda, Begin, Let, LetStar, SetBang, While,

    Display, Newline, Readline, Load, String_q, StringAppend, StringLength,

    Unknown,
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
inline shared_ptr<SExpression> localEnv = makeNil();
const inline shared_ptr<SExpression> TRUE_ = makeAtom("T");
const inline shared_ptr<SExpression> FALSE_ = makeNil();

inline shared_ptr<SExpression> makeString(string value) {
    shared_ptr<SExpression> str = make_shared<SExpression>();

    str->type = ExpressionType::String;
    str->atomValue = move(value);
    
    return str;
}

const inline shared_ptr<SExpression> VOID_ = [] {
    auto v = make_shared<SExpression>();

    v->type = ExpressionType::Void;
    return v;
}();

inline bool isNil(shared_ptr<SExpression> node) {
    return node->type == ExpressionType::Nil;
}

inline bool isAtom(shared_ptr<SExpression> node) {
    return node->type == ExpressionType::Atom;
}

inline bool isPair(shared_ptr<SExpression> node) {
    return node->type == ExpressionType::Pair;
}

inline bool isClosure(shared_ptr<SExpression> node) {
    return node->type == ExpressionType::Closure;
}

inline bool isString(shared_ptr<SExpression> node) {
    return node->type == ExpressionType::String;
}

inline bool isVoid(shared_ptr<SExpression> node) {
    return node->type == ExpressionType::Void;
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

        if (curr == '"') {
            return readString();
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

    static bool isWhitespace(char c) {
        return isspace(static_cast<unsigned char>(c)) || c == ',';
    }

    static bool isDelimiter(char c) {
        return isWhitespace(c) || c == '(' || c == ')' || c == '\'' || c == ';' || c == '"';
    }

    void skipWhitespace() {
        while (!atEnd()) {
            char current = peek();

            if (isWhitespace(current)) {
                advance();
            } else if (current == ';') {
                while (!atEnd() && peek() != '\n') {
                    advance();
                }
            } else {
                break;
            }
        }
    }

    shared_ptr<SExpression> readAtom() {
        string result = "";

        while (!atEnd()) {
            char c = peek();
            if (isWhitespace(c) || isDelimiter(c)) {
                break;
            }

            result += advance();
        }

        return makeAtom(result);
    }

    shared_ptr<SExpression> readString() {
        advance();
        string result;

        while(true) {
            if (atEnd()) throw runtime_error("unterminated string literal");

            char c = advance();
            if (c == '"') {
                break;
            }

            if (c == '\\') {
                if (atEnd()) throw runtime_error("unterminated string literal");

                char esc = advance();
                switch(esc) {
                    case 'n': result += '\n'; break;
                    case 't': result += '\t'; break;
                    case '"': result += '"'; break;
                    case '\\': result += '\\'; break;

                    default: throw runtime_error(string("unknown escape: \\") + esc);
                }
            }
            else {
                result += c;
            }
        }
        return makeString(result);
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

    if(isClosure(expr)) return "#<closure>";

    if (isVoid(expr)) return "";

    if (isString(expr)) {
        string out = "\"";
        for (char c : expr->atomValue) {
            if (c == '"') out += "\\\"";
            else if (c == '\\') out += "\\\\";
            else if (c == '\n') out += "\\n";
            else if (c == '\t') out += "\\t";
            else out += c;
        }
        return out + "\"";
    }

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

inline string displayString(const shared_ptr<SExpression> &expr) {
    if (isString(expr)) return expr->atomValue;
    return exprToString(expr);
}

inline int toInteger(const shared_ptr<SExpression> &node) {
    if (!isAtom(node) || !isInteger(node->atomValue)) {
        throw runtime_error("expected a number but got: " + exprToString(node));
    }

    return stoi(node->atomValue);
}

inline void print(const shared_ptr<SExpression> &expr) {
    cout << exprToString(expr);
}

inline shared_ptr<SExpression> list(shared_ptr<SExpression> a, shared_ptr<SExpression> b) {
    return cons(a, cons(b, makeNil()));
}

inline shared_ptr<SExpression> lookupFrame(const shared_ptr<SExpression> &symbol, shared_ptr<SExpression> frame, bool &found) {
    shared_ptr<SExpression> names = car(frame);
    shared_ptr<SExpression> values = car(cdr(frame));

    while (isPair(names) && isPair(values)) {
        if (car(names)->atomValue == symbol->atomValue) {
            found = true;
            return car(values);
        }
        names = cdr(names);
        values = cdr(values);
    }

    found = false;
    return nullptr;
}

inline shared_ptr<SExpression> lookup(shared_ptr<SExpression> symbol) {
    for (auto frames = localEnv; isPair(frames); frames = cdr(frames)) {
        bool found;
        auto val = lookupFrame(symbol, car(frames), found);
        if (found) return val;
    }

    shared_ptr<SExpression> env = rho;
    while(isPair(env)) {
        shared_ptr<SExpression> entry = car(env);
        if (car(entry) -> atomValue == symbol->atomValue) return car(cdr(entry));
        env = cdr(env);
    }

    return symbol;
}

inline shared_ptr<SExpression> findBindingCell(const shared_ptr<SExpression> &symbol) {
    for (auto frames = localEnv; isPair(frames); frames = cdr(frames)) {
        auto names = car(car(frames));
        auto values = car(cdr(car(frames)));

        while (isPair(names) && isPair(values)) {
            if (car(names)->atomValue == symbol->atomValue) return values;
            names = cdr(names);
            values = cdr(values);
        }
    }

    for (auto env = rho; isPair(env); env = cdr(env)) {
        auto entry = car(env);
        
        if (car(entry)-> atomValue == symbol->atomValue) return cdr(entry);

    }

    return nullptr;
}

inline shared_ptr<SExpression> makeClosure(shared_ptr<SExpression> params, shared_ptr<SExpression> body, shared_ptr<SExpression> env) {
    auto c = make_shared<SExpression>();

    c->type = ExpressionType::Closure;
    c->car = cons(params, body);
    c->cdr = env;
    return c;
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

    if (s == "and?") return Operator::And_q;
    if (s == "or?") return Operator::Or_q;
    if (s == "eq?") return Operator::Eq_q;
    if (s == "if") return Operator::If;
    if (s == "cond") return Operator::Cond;
    
    if (s == "add") return Operator::Add;
    if (s == "sub") return Operator::Subtract;
    if (s == "mul") return Operator::Multiply;
    if (s == "div") return Operator::Divide;
    if (s == "rem") return Operator::Remainder;
    
    if (s == "lt") return Operator::LessThan;

    if (s == "function") return Operator::Function;
    if (s == "def") return Operator::Def;
    if (s == "lambda") return Operator::Lambda;
    if (s == "begin") return Operator::Begin;
    if (s == "let") return Operator::Let;
    if (s == "let*") return Operator::LetStar;
    if (s == "set!") return Operator::SetBang;
    if (s == "while") return Operator::While;

    if (s == "display") return Operator::Display;
    if (s == "newline") return Operator::Newline;
    if (s == "read-line") return Operator::Readline;
    if (s == "load") return Operator::Load;
    if (s == "string?") return Operator::String_q;
    if (s == "string-append") return Operator::StringAppend;
    if (s == "string-length") return Operator::StringLength;

    return Operator::Unknown;
}

struct LocalEnvGuard {
    shared_ptr<SExpression> saved = localEnv;
    ~LocalEnvGuard() { localEnv = saved; }
};

inline shared_ptr<SExpression> eval(shared_ptr<SExpression> expression);

inline shared_ptr<SExpression> evalList(shared_ptr<SExpression> args) {
    if (isNil(args)) {
        return makeNil(); 
    }

    return cons(eval(car(args)), evalList(cdr(args)));
} 

inline shared_ptr<SExpression> eval(shared_ptr<SExpression> expression) {
    LocalEnvGuard guard;    

    while (true) {
        if (isAtom(expression)) return lookup(expression);
        if (isNil(expression)) return expression;
        if (isString(expression) || isVoid(expression)) return expression;

        shared_ptr<SExpression> operation = car(expression);

        if (isPair(operation)) {
            shared_ptr<SExpression> fn = eval(operation);
            if (!isClosure(fn)) return expression;

            shared_ptr<SExpression> actuals = evalList(cdr(expression));
            localEnv = cons(list(car(car(fn)), actuals), cdr(fn));
            expression = cons(makeAtom("begin"), cdr(car(fn)));
            continue;

        } 

        if (!isAtom(operation)) {
            return expression;
        }

        switch(toOp(operation->atomValue)) {
            case Operator::Quote: {
                shared_ptr<SExpression> arg = car(cdr(expression));
                return arg;
            }
            case Operator::Car: {
                shared_ptr<SExpression> arg = eval(car(cdr(expression)));
                if (!isPair(arg)) throw runtime_error("car not a pair: " + exprToString(arg));
                return car(arg);
            }
            case Operator::Cdr: {
                shared_ptr<SExpression> arg = eval(car(cdr(expression)));
                if (!isPair(arg)) throw runtime_error("cdr not a pair: " + exprToString(arg));
                return cdr(arg);
            }
            case Operator::Cons: {
                shared_ptr<SExpression> a = car(cdr(expression));
                shared_ptr<SExpression> b = car(cdr(cdr(expression)));
                return cons(eval(a), eval(b));  
            }
            case Operator::Eval: {
                shared_ptr<SExpression> arg = car(cdr(expression));
                expression = eval(arg);
                continue;
            }
            case Operator::Set: {
                shared_ptr<SExpression> name = car(cdr(expression));
                shared_ptr<SExpression> value = car(cdr(cdr(expression)));

                shared_ptr<SExpression> value_result = eval(value);

                rho = cons(list(name, value_result), rho);
                return value_result;
            }

            //---
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

            //---
            case Operator::And_q: {
                shared_ptr<SExpression> a = car(cdr(expression));
                shared_ptr<SExpression> b = car(cdr(cdr(expression)));

                shared_ptr<SExpression> aRes = eval(a);

                if (isNil(aRes)) {
                    return FALSE_;
                } else {
                    shared_ptr<SExpression> bRes = eval(b);
                    if (isNil(bRes)) {
                        return FALSE_;
                    } else {
                        return TRUE_;
                    }

                }
            }
            case Operator::Or_q: {
                shared_ptr<SExpression> a = car(cdr(expression));
                shared_ptr<SExpression> b = car(cdr(cdr(expression)));

                shared_ptr<SExpression> aRes = eval(a);
                
                if (!isNil(aRes)) {
                    return TRUE_;
                } else {
                    shared_ptr<SExpression> bRes = eval(b);
                    if (!isNil(bRes)) {
                        return TRUE_;
                    } else {
                        return FALSE_;
                    }
                }
            }
            case Operator::Eq_q: {
                shared_ptr<SExpression> a = car(cdr(expression));
                shared_ptr<SExpression> b = car(cdr(cdr(expression)));
                shared_ptr<SExpression> aRes = eval(a);
                shared_ptr<SExpression> bRes = eval(b);

                if (isString(aRes) && isString(bRes)) {
                    return aRes->atomValue == bRes->atomValue ? TRUE_ : FALSE_;
                } 

                if (!isAtom(aRes) || !isAtom(bRes)) {
                    return FALSE_;
                } else {
                    return aRes->atomValue == bRes->atomValue ? TRUE_ : FALSE_;
                }
            }
            case Operator::If: {
                shared_ptr<SExpression> condition = car(cdr(expression));
                shared_ptr<SExpression> evalIfTrue = car(cdr(cdr(expression)));
                shared_ptr<SExpression> elsePart = cdr(cdr(cdr(expression)));

                if (!isNil(eval(condition))) {
                    expression = evalIfTrue;
                    continue;
                }
    
                if (isNil(elsePart)) {
                    return FALSE_;              
                }
                expression = car(elsePart);
                continue;
            }
            case Operator::Cond: {
                shared_ptr<SExpression> clauses = car(cdr(expression));
                shared_ptr<SExpression> chosen = nullptr;
                while (isPair(clauses)) {
                    shared_ptr<SExpression> a = car(clauses);
                    shared_ptr<SExpression> b = car(cdr(clauses));

                    if (!isNil(eval(a))) {
                        chosen = b;
                        break;
                    }

                    clauses = cdr(cdr(clauses));
                }

                if (!chosen) throw runtime_error("no cond clause matched");
                expression = chosen;
                continue;
            }

            //---
            case Operator::Add: {
                shared_ptr<SExpression> a = car(cdr(expression));
                shared_ptr<SExpression> b = car(cdr(cdr(expression)));

                int res = toInteger(eval(a)) + toInteger(eval(b));

                return makeAtom(to_string(res));
            }
            case Operator::Subtract: {
                shared_ptr<SExpression> a = car(cdr(expression));
                shared_ptr<SExpression> b = car(cdr(cdr(expression)));

                int res = toInteger(eval(a)) - toInteger(eval(b));

                return makeAtom(to_string(res));
            }

            case Operator::Multiply: {
                shared_ptr<SExpression> a = car(cdr(expression));
                shared_ptr<SExpression> b = car(cdr(cdr(expression)));

                int res = toInteger(eval(a)) * toInteger(eval(b));

                return makeAtom(to_string(res));
            }

            case Operator::Divide: {
                shared_ptr<SExpression> a = car(cdr(expression));
                shared_ptr<SExpression> b = car(cdr(cdr(expression)));

                int bAsInt = toInteger(eval(b));
                if (bAsInt == 0) {
                    throw runtime_error("divide by zero error");
                }

                int res = toInteger(eval(a)) / bAsInt;

                return makeAtom(to_string(res));
            }

            case Operator::Remainder: {
                shared_ptr<SExpression> a = car(cdr(expression));
                shared_ptr<SExpression> b = car(cdr(cdr(expression)));

                int bAsInt = toInteger(eval(b));

                if (bAsInt == 0) {
                    throw runtime_error("remainder by zero error");
                }

                int res = toInteger(eval(a)) % bAsInt;

                return makeAtom(to_string(res));
            }

            case Operator::LessThan: {
                shared_ptr<SExpression> a = car(cdr(expression));
                shared_ptr<SExpression> b = car(cdr(cdr(expression)));

                return toInteger(eval(a)) < toInteger(eval(b)) ? TRUE_ : FALSE_;
            }

            //---
            case Operator::Function: {
                return expression;
            }
            case Operator::Def: {
                shared_ptr<SExpression> name = car(cdr(expression));
                shared_ptr<SExpression> parameters = car(cdr(cdr(expression)));
                shared_ptr<SExpression> body = car(cdr(cdr(cdr(expression))));
                

                shared_ptr<SExpression> functionExpr = cons(makeAtom("function"), cons(parameters, cons(body, makeNil())));

                shared_ptr<SExpression> setExpr = cons(makeAtom("set"), cons(name, cons(functionExpr, makeNil())));
                return eval(setExpr);
            }

            case Operator::Lambda: {
                return makeClosure(car(cdr(expression)), cdr(cdr(expression)), localEnv);
            }
            
            case Operator::Begin: {
                shared_ptr<SExpression> forms = cdr(expression);
                if (isNil(forms)) return FALSE_;

                while (isPair(cdr(forms))) {
                    eval(car(forms));
                    forms = cdr(forms);
                }
                expression = car(forms);
                continue;
            }

            case Operator::Let: {
                shared_ptr<SExpression> bindings = car(cdr(expression));
                shared_ptr<SExpression> body = cdr(cdr(expression));

                vector<shared_ptr<SExpression>> names, values;
                
                for (auto b = bindings; isPair(b); b = cdr(b)) {
                    names.push_back(car(car(b)));
                    values.push_back(car(cdr(car(b))));
                }

                shared_ptr<SExpression> nameList = makeNil(), valueList = makeNil();

                for (size_t i = names.size(); i-- > 0;) {
                    nameList = cons(names[i], nameList);
                    valueList = cons(values[i], valueList);
                }

                shared_ptr<SExpression> lambda = cons(makeAtom("lambda"), cons(nameList, body));
                expression = cons(lambda, valueList);
                continue;
            }

            case Operator::LetStar: {
                shared_ptr<SExpression> bindings = car(cdr(expression));

                shared_ptr<SExpression> body = cdr(cdr(expression));

                if (!isPair(bindings) || isNil(cdr(bindings))) {
                    expression = cons(makeAtom("let"), cons(bindings, body));
                    continue;
                }

                shared_ptr<SExpression> inner = cons(makeAtom("let*"), cons(cdr(bindings), body));
                expression = cons(makeAtom("let"), cons(cons(car(bindings), makeNil()), cons(inner, makeNil())));
                continue;

            }

            case Operator::SetBang: {
                shared_ptr<SExpression> name = car(cdr(expression));
                shared_ptr<SExpression> value = eval(car(cdr(cdr(expression))));

                shared_ptr<SExpression> cell = findBindingCell(name);
                if (!cell) throw runtime_error("set! of unbound variable: " + name->atomValue);

                cell -> car = value;
                return value;
            }

            case Operator::While: {
                shared_ptr<SExpression> condition = car(cdr(expression));
                shared_ptr<SExpression> body = cdr(cdr(expression));

                while(!isNil(eval(condition))) {
                    for (auto form = body; isPair(form); form = cdr(form)) {
                        eval(car(form));
                    }
                }
                return FALSE_;
            }


            case Operator::Display: {
                for (auto arg = cdr(expression); isPair(arg); arg = cdr(arg)) {
                    cout << displayString(eval(car(arg)));
                }

                cout << flush;
                return VOID_;
            }

            case Operator::Newline: {
                cout << "\n" << flush;
                return VOID_;
            }

            case Operator::Readline: {
                string line;
                if (!getline(cin, line)) return FALSE_;
                return makeString(line);
            }

            case Operator::Load: {
                shared_ptr<SExpression> path = eval(car(cdr(expression)));
                if (!isString(path)) throw runtime_error("load expects a string path, got: " + exprToString(path));

                ifstream file(path->atomValue);
                if (!file) throw runtime_error("could not open file: " + path->atomValue);

                stringstream buffer;
                buffer << file.rdbuf();

                Reader reader(buffer.str());
                while (reader.hasMoreStuff()) {
                    eval(reader.read());
                }

                return VOID_;
            }

            case Operator::String_q: {
                return isString(eval(car(cdr(expression)))) ? TRUE_ : FALSE_;            
            }

            case Operator::StringAppend: {
                string result;
                for (auto arg = cdr(expression); isPair(arg); arg = cdr(arg)) {
                    shared_ptr<SExpression> value = eval(car(arg));

                    if (!isString(value)) throw runtime_error("string-append expects strings, got: " + exprToString(value));

                    result += value->atomValue;
                }

                return makeString(result);
            }

            case Operator::StringLength: {
                shared_ptr<SExpression> value = eval(car(cdr(expression)));
                if (!isString(value)) throw runtime_error("string-length expects a string, got: " + exprToString(value));

                return makeAtom(to_string(value->atomValue.size()));
            }

            case Operator::Unknown: {
                shared_ptr<SExpression> fn = lookup(operation);

                if (isClosure(fn)) {
                    shared_ptr<SExpression> actuals = evalList(cdr(expression));
                    localEnv = cons(list(car(car(fn)), actuals), cdr(fn));
                    expression = cons(makeAtom("begin"), cdr(car(fn)));
                    continue;
                }

                if (!isPair(fn) || car(fn)->atomValue != "function") return expression;

                shared_ptr<SExpression> params = car(cdr(fn));
                shared_ptr<SExpression> body = car(cdr(cdr(fn)));

                shared_ptr<SExpression> actuals = evalList(cdr(expression));

                localEnv = cons(list(params, actuals), makeNil());
                expression = body;
                continue;
            }
        }
        
    } //while
    
}