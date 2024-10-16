#include "parser.h"
#include "machine.h"
#include "errorreporting.h"

#include "astprinter.h"
#include "bcgen.h"

#include <fmt/core.h>
#include <string>
#include <iostream>

// Interpreter
class Interpreter {
    ConstPool constPool;
    Machine machine;

public:
    void interpret(std::string input) {
        const bool debugTokens = true;
        const bool debugAST = true;
        const bool debugBC = true;

        Tokenizer tokenizer(input);
        tokenizer.debug = debugTokens;
        Parser parser(tokenizer);

        ASTPtr root = parser.parse();

        if (ErrorReporter::hasError()) {
            for(auto& report : ErrorReporter::reports()) {
				fmt::print("Error: {}:{} {}\n", report.file, report.line, report.message);
			}
            ErrorReporter::clear();
        }
        else if (debugAST && root) {
            ASTPrinter printer;
            printer.print(root);
        }

        if (root) {
            std::vector<Instruction> instructions;
            BCGenerator bcgen(instructions, constPool);
            bcgen.generate(*root);
            
            if (debugBC)
                machine.dump(instructions, constPool);

            machine.execute(instructions, constPool);
            if (machine.hasError()) {
                fmt::print("Machine error: {}\n", machine.errorMessage());
            }

#if true
            if (machine.stack.size()) {
                fmt::print("Result = {}\n", machine.stack[0].toString());
            }
            else {
                fmt::print("Stack empty.\n");
            }
#endif
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
#if defined(_DEBUG) && defined(_WIN32)
    _CrtMemState s1, s2, s3;
    _CrtMemCheckpoint(&s1);
#endif
    
    Machine::test();
    Tokenizer::test();

    {
        Interpreter interpreter;
        std::string line;

        while (true) {
            fmt::print(">> ");
            std::getline(std::cin, line);
            if (line == "exit") break;
            interpreter.interpret(line);
        }
    }

#if defined(_DEBUG) && defined(_WIN32)
    int knownNumLeak = 0;
    int knownLeakSize = 0;

    _CrtMemCheckpoint(&s2);
    _CrtMemDifference(&s3, &s1, &s2);
    _CrtMemDumpStatistics(&s3);

    auto leakCount = s3.lCounts[1];
    auto leakSize = s3.lSizes[1];
    auto totalAllocaton = s3.lTotalCount;
    auto highWater = s3.lHighWaterCount / 1024;

    fmt::print("Memory report:\n");
    fmt::print("Leak count = {}  size = {}\n", leakCount, leakSize);
    fmt::print("High water = {}kb  number of allocations = {}\n", highWater, totalAllocaton);
    assert(s3.lCounts[1] <= knownNumLeak);
    assert(s3.lSizes[1] <= knownLeakSize);
#endif

    return 0;
}
