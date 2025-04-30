int main() {
    // Example Lisp program: ((lambda (x) (+ x 1)) 5)
    std::string program = "((lambda (x) (+ x 1)) 5)";

    // Tokenize → Parse → Evaluate
    auto tokens = tokenize(program);
    auto ast = parse(tokens);

    // Set up global environment with built-in functions
    Env env;
    env["+"] = [](std::vector<Value> args) { return to_int(args[0]) + to_int(args[1]); };
    env["-"] = [](std::vector<Value> args) { return to_int(args[0]) - to_int(args[1]); };
    env["*"] = [](std::vector<Value> args) { return to_int(args[0]) * to_int(args[1]); };
    env["/"] = [](std::vector<Value> args) { return to_int(args[0]) / to_int(args[1]); };
    env[">"] = [](std::vector<Value> args) { return to_int(args[0]) > to_int(args[1]) ? 1 : 0; };

    // Evaluate and print result
    try {
        Value result = eval(ast, env);
        if (std::holds_alternative<int>(result))
            std::cout << "Result: " << std::get<int>(result) << "\n";
        else if (std::holds_alternative<std::string>(result))
            std::cout << "Result: " << std::get<std::string>(result) << "\n";
        else
            std::cout << "Result: <function>\n";
    } catch (const std::exception& e) {
        std::cerr << "Execution Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}