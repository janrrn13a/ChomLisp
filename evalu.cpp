using Value = std::variant<int, std::string, std::function<Value(std::vector<Value>)>>;
using Env = std::map<std::string, Value>;

Value eval(ExprPtr expr, Env& env);

int to_int(const Value& v) {
    if (std::holds_alternative<int>(v)) return std::get<int>(v);
    throw std::runtime_error("Expected number");
}

std::string to_sym(const Value& v) {
    if (std::holds_alternative<std::string>(v)) return std::get<std::string>(v);
    throw std::runtime_error("Expected symbol");
}

Value eval_list(const std::vector<ExprPtr>& list, Env& env) {
    if (list.empty()) throw std::runtime_error("Empty expression");
    Atom* head = dynamic_cast<Atom*>(list[0].get());
    if (!head) throw std::runtime_error("Expected function name");

    std::string sym = head->value;

    if (sym == "define") {
        auto* name = dynamic_cast<Atom*>(list[1].get());
        Value val = eval(list[2], env);
        env[name->value] = val;
        return val;
    } else if (sym == "lambda") {
        auto* args_list = dynamic_cast<List*>(list[1].get());
        std::vector<std::string> params;
        for (auto& a : args_list->elements)
            params.push_back(dynamic_cast<Atom*>(a.get())->value);
        ExprPtr body = list[2];
        return [params, body](std::vector<Value> args) -> Value {
            Env local;
            for (size_t i = 0; i < params.size(); ++i)
                local[params[i]] = args[i];
            return eval(body, local);
        };
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
    if (auto* a = dynamic_cast<Atom*>(expr.get())) {
        if (isdigit(a->value[0]) || (a->value[0] == '-' && a->value.size() > 1))
            return std::stoi(a->value);
        if (env.count(a->value)) return env[a->value];
        return a->value; // treat as symbol
    } else if (auto* l = dynamic_cast<List*>(expr.get())) {
        return eval_list(l->elements, env);
    }
    throw std::runtime_error("Unknown expression");
}