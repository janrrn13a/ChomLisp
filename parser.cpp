struct Expr{
    virtual ~Expr() = default;
}

using ExprPtr = std::shared_ptr<Expr>;

struct Atom : Expr{
    std::string value;
    Atom(std::string value) : value(value) {}
};

struct List : Expr{
    std::vector<ExprPtr> elements;
    List(std::vector<ExprPtr> elements) : elements(std::move(elements)) {}
};

ExprPtr parse_expr(std::vector<Token>& tokens, size_t& pos){
    if (tokens[pos].type == TokenType::NUMBER || tokens[pos].type == TokenType::SYMBOL){
        return std::make_shared<Atom>(tokens[pos++].value);
    } else if (tokens[pos].type == TokenType::LPAREN){
        pos++; // consume '('
        std::vector<ExprPtr> elements;
        while (tokens[pos].type != TokenType::RPAREN){
            elements.push_back(parse_expr(tokens, pos));
        }
        pos++; // consume ')'
        return std::make_shared<List>(elements);
    } else {
        throw std::runtime_error("Unexpected token");
    }  
}

ExprPtr parse(const std::vector<Token>& tokens){
    size_t pos = 0;
    return parse_expr(tokens, pos);
}