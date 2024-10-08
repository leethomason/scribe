#include "parser.h"
#include "machine.h"

#include <iostream>
#include <string>
#include <map>
#include <cctype>
#include <stdexcept>
#include <sstream>

// Interpreter
class Interpreter {
    ConstPool constPool;
    Machine machine;

public:
    void interpret(std::string input) {
        try {
            Tokenizer tokenizer(input);
            Parser parser;
            ASTNode* root = parser.parse(tokenizer);

            std::vector<Instruction> instructions;

            root->evaluate(instructions, constPool);
            delete root; root = 0;

            machine.execute(instructions, constPool);

            //std::cout << "Result: " << result << std::endl;
        } catch (std::exception& ex) {
            std::cout << "Error: " << ex.what() << std::endl;
        }
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
        std::cout << ">> ";
        std::getline(std::cin, line);
        if (line == "exit") break;
        interpreter.interpret(line);
    }
    return 0;
}
