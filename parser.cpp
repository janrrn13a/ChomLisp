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