#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <memory>
#include <functional>
#include <regex>

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

std::string tokenTypeToString(TokenType t) {
    switch (t) {
        case NUMBER: return "NUMBER";
        case STRING: return "STRING";
        case IDENTIFIER: return "IDENTIFIER";
        case OPERATOR: return "OPERATOR";
        case KEYWORD: return "KEYWORD";
        case PUNCTUATION: return "PUNCTUATION";
        case COMMENT: return "COMMENT";
        case WHITESPACE: return "WHITESPACE";
        case OPEN_PAREN: return "OPEN_PAREN";
        case CLOSE_PAREN: return "CLOSE_PAREN";
        default: return "UNKNOWN";
    }
}

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
    return std::regex_match(s, std::regex("^[a-zA-Z_+\\-\\*/><=!?][a-zA-Z0-9_+\\-\\*/><=!?]*$"));
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

std::vector<Token> tokenize(const std::string& sourceCode) {
    std::vector<Token> tokens;
    std::vector<std::string> src = splitString(sourceCode); // splitString already works with const std::string&

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

    for (const auto& t : tokens) {
        std::cout << "Token: " << t.value << " (" << tokenTypeToString(t.type) << ")\n";
    }

    return tokens;
}


// Parser
struct Expr {
    virtual ~Expr() {}
};

typedef Expr* ExprPtr;

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
        return new Atom(tokens[pos++].value);
    } else if (tokens[pos].type == OPEN_PAREN) {
        ++pos;
        std::vector<ExprPtr> elements;
        while (tokens[pos].type != CLOSE_PAREN) {
            elements.push_back(parse_expr(tokens, pos));
        }
        ++pos;
        return new List(elements);
    } else {
        throw std::runtime_error("Unexpected token: " + tokens[pos].value);
    }
}

ExprPtr parse(std::vector<Token>& tokens) {
    size_t pos = 0;
    return parse_expr(tokens, pos);
}

// Evaluator
typedef std::function<int(std::vector<int>)> ValueFunc;
typedef std::map<std::string, ValueFunc> Env;

int eval(ExprPtr expr, Env& env);

int eval_list(const std::vector<ExprPtr>& list, Env& env) {
    if (list.empty()) throw std::runtime_error("Empty list");

    Atom* head = dynamic_cast<Atom*>(list[0]);
    if (!head) throw std::runtime_error("Expected function name");

    std::string sym = head->value;
    std::cout << "[eval_list] Function: " << sym << "\n";

    if (sym == "define") {
        Atom* name = dynamic_cast<Atom*>(list[1]);
        int val = eval(list[2], env);
        env[name->value] = [val](std::vector<int>) { return val; };
        std::cout << "[define] " << name->value << " = " << val << "\n";
        return val;
    } else if (sym == "if") {
        int cond = eval(list[1], env);
        return cond ? eval(list[2], env) : eval(list[3], env);
    } else if (sym == "quote") {
        std::cout << "[quote] Returning raw expression\n";
        if (Atom* atom = dynamic_cast<Atom*>(list[1])) {
            std::cout << "[quote] Atom: " << atom->value << "\n";
            return 0; // or return placeholder value
        } else if (List* quotedList = dynamic_cast<List*>(list[1])) {
            std::cout << "[quote] List: (";
            for (auto& item : quotedList->elements) {
                if (Atom* a = dynamic_cast<Atom*>(item)) {
                    std::cout << a->value << " ";
                } else {
                    std::cout << "<expr> ";
                }
            }
            std::cout << ")\n";
            return 0; // no evaluation, just acknowledgment
        } else {
            throw std::runtime_error("Unsupported quote content");
        }
    }

    if (!env.count(sym)) {
        throw std::runtime_error("Undefined function: " + sym);
    }

    ValueFunc func = env[sym];
    std::vector<int> args;
    for (size_t i = 1; i < list.size(); ++i)
        args.push_back(eval(list[i], env));

    std::cout << "[call] " << sym << " with args:";
    for (int a : args) std::cout << " " << a;
    std::cout << "\n";

    return func(args);
}

int eval(ExprPtr expr, Env& env) {
    if (Atom* a = dynamic_cast<Atom*>(expr)) {
        std::cout << "[eval atom] " << a->value << "\n";
        if (std::regex_match(a->value, std::regex("^-?\\d+$"))) {
            return std::stoi(a->value);
        }
        if (env.count(a->value)) {
            return env[a->value]({});
        }
        throw std::runtime_error("Undefined variable or symbol: " + a->value);
    } else if (List* l = dynamic_cast<List*>(expr)) {
        std::cout << "[eval list]\n";
        return eval_list(l->elements, env);
    }
    throw std::runtime_error("Unknown expression type");
}

// Main
int main() {
    std::vector<std::string> programs = {
        "(define x 5)",
        "(+ x 3)",
        "(quote (1 2 3))",
        "(if (> x 3) (+ x 2) (- x 2))",
        "(square 4)"                             
    };

    Env env;
    env["+"] = [](std::vector<int> args) { return args[0] + args[1]; };
    env["-"] = [](std::vector<int> args) { return args[0] - args[1]; };
    env["*"] = [](std::vector<int> args) { return args[0] * args[1]; };
    env["/"] = [](std::vector<int> args) { return args[0] / args[1]; };
    env[">"] = [](std::vector<int> args) { return args[0] > args[1]; };
    env["square"] = [](std::vector<int> args) { return args[0] * args[0]; };

    try {
        for (size_t i = 0; i < programs.size(); ++i) {
            const std::string& program = programs[i];
            std::cout << "==============================\n";
            std::cout << "Program " << (i + 1) << ": " << program << "\n";

            std::vector<Token> tokens = tokenize(program);
            std::cout << "Tokens: ";
            for (const auto& t : tokens)
                std::cout << t.value << " ";
            std::cout << "\n";

            ExprPtr ast = parse(tokens);
            std::cout << "Evaluating...\n";
            int result = eval(ast, env);
            std::cout << "Result: " << result << "\n";
            std::cout << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
