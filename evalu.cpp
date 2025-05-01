typedef std::function<int(std::vector<int>)> ValueFunc;
typedef std::map<std::string, ValueFunc> Env;
int eval(ExprPtr expr, Env& env);

int to_int(const std::string& v) {
    try {
        return std::stoi(v);
    } catch (const std::invalid_argument&) {
        throw std::runtime_error("Invalid number format: " + v);
    } catch (const std::out_of_range&) {
        throw std::runtime_error("Number out of range: " + v);
    }
}

int eval_list(const std::vector<ExprPtr>& list, Env& env) {
    if (list.empty()) throw std::runtime_error("Empty list");

    Atom* head = dynamic_cast<Atom*>(list[0]);
    if (!head) throw std::runtime_error("Expected function name");

    std::string sym = head->value;

    if (sym == "define") {
        Atom* name = dynamic_cast<Atom*>(list[1]);
        int val = eval(list[2], env);
        env[name->value] = [val](std::vector<int>) { return val; }; // wrap int in function
        return val;
    } else if (sym == "if") {
        int cond = eval(list[1], env);
        return cond ? eval(list[2], env) : eval(list[3], env);
    }

    if (!env.count(sym)) throw std::runtime_error("Unknown function: " + sym);
    ValueFunc func = env[sym];
    std::vector<int> args;
    for (size_t i = 1; i < list.size(); ++i)
        args.push_back(eval(list[i], env));
    return func(args);
}

int eval(ExprPtr expr, Env& env) {
    if (Atom* a = dynamic_cast<Atom*>(expr)) {
        if (isNumber(a->value)) {
            return to_int(a->value);
        }
        if (env.count(a->value)) {
            return env[a->value]({}); // call stored function with no args
        }
        throw std::runtime_error("Undefined symbol: " + a->value);
    } else if (List* l = dynamic_cast<List*>(expr)) {
        return eval_list(l->elements, env);
    }
    throw std::runtime_error("Unknown expression");
}