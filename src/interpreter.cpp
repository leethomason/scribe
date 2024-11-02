#include "interpreter.h"

#include "token.h"
#include "parser.h"
#include "errorreporting.h"
#include "astprinter.h"
#include "bcgen.h"

void Interpreter::interpret(std::string input)
{
    const bool debugTokens = true;
    const bool debugAST = true;
    const bool debugBC = true;

    Tokenizer tokenizer(input);
    tokenizer.debug = debugTokens;
    Parser parser(tokenizer);

    ASTExprPtr root = parser.parseExpr();

    if (ErrorReporter::hasError()) {
        for (auto& report : ErrorReporter::reports()) {
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
