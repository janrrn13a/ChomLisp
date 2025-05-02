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