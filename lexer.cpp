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


enum TokenType{
    NONE,
    NUMBER,
    STRING,
    IDENTIFIER,
    OPERATOR,
    KEYWORD,
    PUNCTUATION,
    COMMENT,
    WHITESPACE
    };

struct Token{
    std::string value;
    TokenType type;
};

Token token(const std::string& val, TokenType type) {
    return Token{val, type};
}

std::string shift(std::vector<std::string>& src) {
    std::string val = src.front();
    src.erase(src.begin());
    return val;
}

std::vector<Token> tokenize(std::string &sourceCode) {
    std::vector<Token> tokens;
    std::vector<std::string> src = splitString(sourceCode);

    while (!src.empty()) {
        if (src.front() == "(") {
            tokens.push_back(token(shift(src), TokenType::OpenParen));
        }
        else if (src.front() == ")") {
            tokens.push_back(token(shift(src), TokenType::CloseParen));
        }
        else if (isNumber(src.front())) {
            std::string number;
            while (!src.empty() && isNumber(src.front())) {
                number += shift(src);
            }
            tokens.push_back(token(number, TokenType::Number));
        }
        else if (isString(src.front())) {
            std::string str;
            while (!src.empty() && isString(src.front())) {
                str += shift(src);
            }
            tokens.push_back(token(str, TokenType::String));
        }
        else if (isIdentidfier(src.front())) {
            std::string ident = shift(src);
            while (!src.empty() && isIdentidfier(src.front())) {
                ident += shift(src);
            }
            tokens.push_back(token(ident, TokenType::Identifier));
        }
        else if (isOperator(src.front())) {
            tokens.push_back(token(shift(src), TokenType::Operator));
        }
        else if (isKeyword(src.front())) {
            tokens.push_back(token(shift(src), TokenType::Keyword));
        }
        else if (isPunctuation(src.front())) {
            tokens.push_back(token(shift(src), TokenType::Punctuation));
        }
        else if (isComment(src.front())) {
            std::string comment;
            while (!src.empty() && isComment(src.front())) {
                comment += shift(src);
            }
            tokens.push_back(token(comment, TokenType::Comment));
        }
        else if (isWhitespace(src.front()[0])) {
            shift(src);
        }
        } else {
            throw std::runtime_error("Unknown token: " + src.front());
        }
    }

    return tokens;
}

std::vector<std::string> splitString(const std::string &sourceCode) {
    std::vector<std::string> words;
    std::string word;

    for (char ch : sourceCode) {
        if (ch != ' ') {
            word += ch;
        } else if (!word.empty()) {
            words.push_back(word);
            word.clear();
        }
    }

    if (!word.empty()) {
        words.push_back(word);
    }

    return words;
}