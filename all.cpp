#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <regex>
#include <algorithm>
#include <cctype>
#include <iterator>
#include <stdexcept>
#include <utility>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <variant>
#include <memory>
#include <functional>

// Tokenizer

enum TokenType {
    NONE,
    NUMBER,
    STRING,
    IDENTIFIER,
    OPERATOR,
    KEYWORD,
    PUNCTUATION,
    COMMENT,
    WHITESPACE,
    OPEN_PAREN,
    CLOSE_PAREN
};

struct Token {
    std::string value;
    TokenType type;
};

std::string shift(std::vector<std::string>& src) {
    std::string front = src.front();
    src.erase(src.begin());
    return front;
}

bool isNumber(const std::string& s) {
    return std::regex_match(s, std::regex("^-?\\d+$"));
}

bool isString(const std::string& s) {
    return s.size() >= 2 && s.front() == '"' && s.back() == '"';
}

bool isIdentifier(const std::string& s) {
    return std::regex_match(s, std::regex("^[a-zA-Z_+\\-*/><=!?][a-zA-Z0-9_+\\-*/><=!?]*$"));
}

bool isSkippable(char ch) {
    return std::isspace(ch);
}

Token token(const std::string& val, TokenType t) {
    return Token{val, t};
}

std::vector<std::string> splitString(const std::string& sourceCode) {
    std::vector<std::string> tokens;
    std::string current;
    for (char ch : sourceCode) {
        if (std::isspace(ch)) {
            if (!current.empty()) {
                tokens.push_back(current);
                current.clear();
            }
        } else if (ch == '(' || ch == ')') {
            if (!current.empty()) {
                tokens.push_back(current);
                current.clear();
            }
            tokens.push_back(std::string(1, ch));
        } else {
            current += ch;
        }
    }
    if (!current.empty()) tokens.push_back(current);
    return tokens;
}

std::vector<Token> tokenize(std::string& sourceCode) {
    std::vector<Token> tokens;
    std::vector<std::string> src = splitString(sourceCode);

    while (!src.empty()) {
        std::string current = src.front();

        if (current == "(") {
            tokens.push_back(token(shift(src), OPEN_PAREN));
        } else if (current == ")") {
            tokens.push_back(token(shift(src), CLOSE_PAREN));
        } else if (isNumber(current)) {
            tokens.push_back(token(shift(src), NUMBER));
        } else if (isString(current)) {
            tokens.push_back(token(shift(src), STRING));
        } else if (isIdentifier(current)) {
            tokens.push_back(token(shift(src), IDENTIFIER));
        } else if (isSkippable(current[0])) {
            shift(src);
        } else {
            throw std::runtime_error("Unknown token: " + current);
        }
    }

    return tokens;
}

// Parser

struct Expr {
    virtual ~Expr() {}
};

using ExprPtr = std::shared_ptr<Expr>;

struct Atom : Expr {
    std::string value;
    Atom(std::string value) : value(std::move(value)) {}
};

struct List : Expr {
    std::vector<ExprPtr> elements;
    List(std::vector<ExprPtr> elements) : elements(std::move(elements)) {}
};

ExprPtr parse_expr(std::vector<Token>& tokens, size_t& pos) {
    if (tokens[pos].type == NUMBER || tokens[pos].type == IDENTIFIER || tokens[pos].type == STRING) {
        return std::make_shared<Atom>(tokens[pos++].value);
    } else if (tokens[pos].type == OPEN_PAREN) {
        ++pos;
        std::vector<ExprPtr> elements;
        while (tokens[pos].type != CLOSE_PAREN) {
            elements.push_back(parse_expr(tokens, pos));
        }
        ++pos;
        return std::make_shared<List>(elements);
    } else {
        throw std::runtime_error("Unexpected token: " + tokens[pos].value);
    }
}

ExprPtr parse(std::vector<Token>& tokens) {
    size_t pos = 0;
    return parse_expr(tokens, pos);
}

// Evaluator

using Value = std::variant<int, std::string, std::function<Value(std::vector<Value>)>>;
using Env = std::map<std::string, Value>;

Value eval(ExprPtr expr, Env& env);

int to_int(const Value& v) {
    if (std::holds_alternative<int>(v)) return std::get<int>(v);
    throw std::runtime_error("Expected number");
}

std::string to_str(const Value& v) {
    if (std::holds_alternative<std::string>(v)) return std::get<std::string>(v);
    throw std::runtime_error("Expected string");
}

Value eval_list(const std::vector<ExprPtr>& list, Env& env) {
    if (list.empty()) throw std::runtime_error("Empty list");

    Atom* head = dynamic_cast<Atom*>(list[0].get());
    if (!head) throw std::runtime_error("Expected function name");

    std::string sym = head->value;

    if (sym == "define") {
        auto* name = dynamic_cast<Atom*>(list[1].get());
        Value val = eval(list[2], env);
        env[name->value] = val;
        return val;
    } else if (sym == "if") {
        Value cond = eval(list[1], env);
        return to_int(cond) ? eval(list[2], env) : eval(list[3], env);
    }

    Value func = eval(list[0], env);
    std::vector<Value> args;
    for (size_t i = 1; i < list.size(); ++i)
        args.push_back(eval(list[i], env));
    return std::get<std::function<Value(std::vector<Value>)>>(func)(args);
}

Value eval(ExprPtr expr, Env& env) {
    if (Atom* a = dynamic_cast<Atom*>(expr.get())) {
        if (std::regex_match(a->value, std::regex("^-?\\d+$"))) {
            return std::stoi(a->value);
        }
        if (env.count(a->value)) return env[a->value];
        return a->value;
    } else if (List* l = dynamic_cast<List*>(expr.get())) {
        return eval_list(l->elements, env);
    }
    throw std::runtime_error("Unknown expression");
}

int main() {
    std::string program = "((define x 5) (+ x 3))";

    try {
        auto tokens = tokenize(program);
        auto ast = parse(tokens);

        Env env;
        env["+"] = [](std::vector<Value> args) { return to_int(args[0]) + to_int(args[1]); };
        env["-"] = [](std::vector<Value> args) { return to_int(args[0]) - to_int(args[1]); };
        env["*"] = [](std::vector<Value> args) { return to_int(args[0]) * to_int(args[1]); };
        env["/"] = [](std::vector<Value> args) { return to_int(args[0]) / to_int(args[1]); };

        Value result = eval(ast, env);
        if (std::holds_alternative<int>(result))
            std::cout << "Result: " << std::get<int>(result) << "\n";
        else if (std::holds_alternative<std::string>(result))
            std::cout << "Result: " << std::get<std::string>(result) << "\n";
        else
            std::cout << "Result: <function>\n";
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
