#include "interpreter.h"

#include "token.h"
#include "parser.h"
#include "errorreporting.h"
#include "astprinter.h"
#include "bcgen.h"
#include "instinterp.h"

Value Interpreter::interpret(std::string input)
{
    Tokenizer tokenizer(input);
    tokenizer.debug = false;
    Parser parser(tokenizer);
    Value rc;

    std::vector<ASTStmtPtr> stmts = parser.parseStmts();

    if (ErrorReporter::hasError()) {
        ErrorReporter::printReports();
        ErrorReporter::clear();
        return rc;
    }

    for (const auto& stmt : stmts) {
        stmt->accept(*this);
    }
    return rc;
}
