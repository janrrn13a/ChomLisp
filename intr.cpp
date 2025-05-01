int main() {
    std::string program = "((define x 5) (+ x 3))";

    try {
        auto tokens = tokenize(program);
        for (const auto& token : tokens) {
            std::cout << "Token: " << token.value << "\n";
        }
        std::cout << "Parsing..." << std::endl;
        auto ast = parse(tokens);
        std::cout << "Parsed successfully." << std::endl;
        std::cout << "Evaluating...\n";


        Env env;
        env["x"] = [](std::vector<int> args) { return args[0]; };
        env["+"] = [](std::vector<int> args) { return args[0] + args[1]; };
        env["-"] = [](std::vector<int> args) { return args[0] - args[1]; };
        env["*"] = [](std::vector<int> args) { return args[0] * args[1]; };
        env["/"] = [](std::vector<int> args) { return args[0] / args[1]; };

        int result = eval(ast, env);
        std::cout << "Result: " << result << "\n";
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
