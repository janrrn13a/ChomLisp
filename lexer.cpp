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

    for (const auto& t : tokens) {
        std::cout << "Token: " << t.value << " (" << tokenTypeToString(t.type) << ")\n";
    }

    return tokens;
}