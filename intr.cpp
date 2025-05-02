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