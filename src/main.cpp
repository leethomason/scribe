#include "errorreporting.h"
#include "interpreter.h"
#include "token.h"
#include "langtest.h"
#include "machine.h"

#include <fmt/core.h>
#include <string>
#include <iostream>

// Interpreter

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
    LangTest();

    {
        Interpreter interpreter;
        std::string line;

        while (true) {
            fmt::print(">> ");
            std::getline(std::cin, line);
            if (line == "exit") break;
            Value rc = interpreter.interpret(line);

            if (ErrorReporter::hasError()) {
                ErrorReporter::printReports();
                ErrorReporter::clear();
            }

            if (rc.type != ValueType::tNone) {
				fmt::print("Result: {}\n", rc.toString());
			}
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
