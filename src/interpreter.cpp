#include "interpreter.h"

#include "token.h"
#include "parser.h"
#include "errorreporting.h"
#include "astprinter.h"
#include "bcgen.h"
#include "instinterp.h"

Value Interpreter::interpret(std::string input)
{
    const bool debugTokens = true;
    //const bool debugAST = true;
    //const bool debugBC = true;

    Tokenizer tokenizer(input);
    tokenizer.debug = debugTokens;
    Parser parser(tokenizer);
    Value rc;

    std::vector<ASTStmtPtr> stmts = parser.parseStmts();

    if (ErrorReporter::hasError()) {
        for (auto& report : ErrorReporter::reports()) {
            fmt::print("Error: {}:{} {}\n", report.file, report.line, report.message);
        }
        ErrorReporter::clear();
        return rc;
    }

#if 0
    // FIXME: switch back to byte code
    // Using an interpreter for dev.
    std::vector<Instruction> instructions;
    BCStmtGenerator bcgen(instructions, constPool);
#else
    StmtInterpreter iMachine;
#endif

    for (const auto& stmt : stmts) {
        // FIXME
        //if (debugAST) {
        //	ASTPrinter printer;
        //	printer.print(stmt);
        //}

#if 0
        bcgen.generate(*stmt);
#else
        iMachine.interpret(*stmt);
#endif
    }
#if 0
    if (debugBC)
        machine.dump(instructions, constPool);
    machine.execute(instructions, constPool);

    if (machine.hasError()) {
        fmt::print("Machine error: {}\n", machine.errorMessage());
    }
    if (machine.stack.size()) {
        rc = machine.stack[0];
    }
    machine.stack.clear();
#endif

    return rc;
}
