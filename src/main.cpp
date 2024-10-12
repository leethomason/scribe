#include "parser.h"
#include "machine.h"

#include <fmt/core.h>
#include <string>
#include <iostream>

// Interpreter
class Interpreter {
    ConstPool constPool;
    Machine machine;

public:
    void interpret(std::string input) {
        const bool debugAST = true;
        const bool debugBC = false;
        const bool debugTokens = false;

        Tokenizer tokenizer(input);
        tokenizer.debug = debugTokens;
        Parser parser;

        ASTPtr root = parser.parse(tokenizer);

        if (parser.hasError()) {
            fmt::print("Parser error: {}\n", parser.errorMessage());
        }
        else if (debugAST)
            root->dump(0);

        std::vector<Instruction> instructions;
        root->evaluate(instructions, constPool);
        if (debugBC)
            machine.dump(instructions, constPool);

        machine.execute(instructions, constPool);
        if (machine.hasError()) {
            fmt::print("Machine error: {}\n", machine.errorMessage());
        }

        if (machine.stack.size()) {
            fmt::print("Result = {}\n", machine.stack[0].toString());
        }
        else {
            fmt::print("Stack empty.\n");
        }
        machine.stack.clear();
    }
};

/*
    var x = 1.0;            // declare x (local)
    var y = 2.0 + x;        // declare y (local) from expr
    return x + y;           // add, push result
*/

int main() 
{
	Machine::test();
    Tokenizer::test();

    Interpreter interpreter;
    std::string line;

    while (true) {
        fmt::print(">> ");
        std::getline(std::cin, line);
        if (line == "exit") break;
        interpreter.interpret(line);
    }
    return 0;
}
