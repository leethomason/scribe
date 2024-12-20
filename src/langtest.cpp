#include "langtest.h"
#include "interpreter.h"
#include "test.h"
#include "errorreporting.h"

#include <string>

// The problem is that runtime errors don't have line numbers. 
// But should! Until then, flag runtime errors.
static constexpr int RUNTIME = 0;	

static void Run(const std::string& s, Value expectedResult = Value(), bool expectedError = false, int errorLine = -1)
{
	Interpreter ip;
	Value r = ip.interpret(s, "langtest");
	
	if (expectedError) {
		TEST(ErrorReporter::hasError());

		const ErrorReporter::Report& report = ErrorReporter::reports().front();
		if (errorLine >= 0) {
			TEST(errorLine == report.line);
		}
	}
	else {
		TEST(!ErrorReporter::hasError());
		TEST(r == expectedResult);
	}
	ErrorReporter::clear();
}

static void PrintRun(const std::string& s, const std::string& expectedPrint, bool expectedError = false, int errorLine = -1)
{
	Interpreter ip;
	Value out = ip.interpret(s, "langtest-print");

	if (expectedError) {
		TEST(ErrorReporter::hasError());
		const ErrorReporter::Report& report = ErrorReporter::reports().front();
		if (errorLine >= 0) {
			TEST(errorLine == report.line);
		}
	}
	else {
		ErrorReporter::printReports();
		TEST(!ErrorReporter::hasError());
	}
	ErrorReporter::clear();

	TEST(out.type.pType == PType::tStr);
	TEST(out.type.layout == Layout::tScalar);
	TEST(*out.vString == expectedPrint);
}

static void SimplePrint()
{
	const std::string s =
		"return format(13)";

	Interpreter ip;
	Value out = ip.interpret(s, "langtest");
	TEST(out.type.pType == PType::tStr);
	TEST(out.type.layout == Layout::tScalar);
	TEST(*out.vString == "13");
}

static void SimpleReturn()
{
	const std::string s =
		"return 13";
	
	Run(s, Value::Number(13));
}

static void SimpleError()
{
	const std::string s =
		"return ; 13";

	Run(s, Value(), true, 0);
}

static void OnePlusTwo()
{
	const std::string s = 
		"return 1 + 2";

    Run(s, Value::Number(3));
}

static void OneMinusTwo()
{
	const std::string s =
		"return 1 - 2";

	Run(s, Value::Number(-1));
}

static void TestParen()
{
	const std::string s = 
		"return (1-2)*3";

	Run(s, Value::Number(-3));
}

static void TestVarDuck()
{
	const std::string s =
		"var x = 8\n"
		"return x";

	Run(s, Value::Number(8));
}

static void TestVarDecl()
{
	const std::string s =
		"var x: num = 9\n"
		"return x";

	Run(s, Value::Number(9));
}

static void TestDuckFail()
{
	const std::string s = 
		"// comment\n"
		"var x = 4 + 5\n"
		"return x";
	Run(s, Value(), true, 1);
}

static void TestHelloWorldVar()
{
	const std::string s =
		"var x = 'Hello'\n"
		"return x + ', World!'";
	Run(s, Value::String("Hello, World!"));
}

static void BadVarSyntax()
{
	const std::string s =
		"// comment 0\n"
		"// comment 1\n"
		"if (true) var x = 1\n";
	Run(s, Value(), true, 2);
}

static void AssignVar()
{
	const std::string s =
		"var x = 1\n"
		"x = 2\n"
		"return x";
	Run(s, Value::Number(2));
}

static void AssignVarPlusOne()
{
	const std::string s =
		"var x = 1\n"
		"x = 2\n"
		"return x + 1";
	Run(s, Value::Number(3));
}

static void NoRedeclare()
{
	const std::string s =
		"var x = 1\n"
		"var x = 2\n"
		"return x";
	Run(s, Value(), true, RUNTIME);
}

static void ScopeTest()
{
	const std::string s =
		"var out: str\n"
		"var a = 'global a'\n"
		"var b = 'global b'\n"
		"var c = 'global c'\n"
		"{\n"
		"	var a = 'outer a'\n"
		"	var b = 'outer b'\n"
		"	{\n"
		"		var a = 'inner a'\n"
		"		out = out + format(a, b, c) + '..'\n"
		"   }\n"
		"	out = out + format(a, b, c) + '..'\n"
		"}\n"
		"return out + format(a, b, c)\n";

	const std::string expected =
		"inner a, outer b, global c..outer a, outer b, global c..global a, global b, global c";
	PrintRun(s, expected);
}

static void AssignmentExpressions()
{
	// Went back and forth on this one.
	// In the end, Swift does allow this (and uses it for declaration)
	// so went with allowing returns from assignment.
	const std::string s =
		"var a = 1\n"
		"var b = 2\n"
		"var c = 3\n"
		"a = b = c\n"
		"return a";
		Run(s, Value::Number(3));
}

static void ShortenedAssignmentExpressions()
{
	// Went back and forth on this one.
	// In the end, Swift does allow this (and uses it for declaration)
	// so went with allowing returns from assignment.
	const std::string s =
		"var a = 1\n"
		"var b = 2\n"
		"var c = 3\n"
		"return a = b = c\n";
	Run(s, Value::Number(3));
}

static void BlockTest()
{
	const std::string s =
		"var a = 1\n"
		"{ a = 17 }\n"
		"return a";
	Run(s, Value::Number(17));
}

static void SimpleParenTest()
{
	const std::string s =
		"return (1 + 16)";
	Run(s, Value::Number(17));
}

static void ParenTest()
{
	const std::string s =
		"var a = 1\n"
		"return (a + 16)";
	Run(s, Value::Number(17));
}

static void BasicIfTest()
{
	const std::string s =
		"var a = 1\n"
		"if a == 1 {\n"
		"	a = 2\n"
		"}\n"
		"return a";
	Run(s, Value::Number(2));
}

static void LogicalOR()
{
	const std::string s =
		"var a: bool = false\n"
		"var b: bool = true\n"
		"var c: str = 'bye'\n"
		"if a || b {\n"
		"	c = 'hi'\n"
		"}\n"
		"return c\n";
	Run(s, Value::String("hi"));
}

static void LogicalAND()
{
	const std::string s =
		"var a: bool = false\n"
		"var b: bool = true\n"
		"var c: str = 'bye'\n"
		"if a && b {\n"
		"	c = 'hi'\n"
		"}\n"
		"return c\n";
	Run(s, Value::String("bye"));
}

static void BasicWhile()
{
	const std::string s =
		"var a = 0\n"
		"var b = 0\n"
		"while a < 10 {\n"
		"  a = a + 1\n"
		"  b = b + 2\n"
		"}\n"
		"return b";
	Run(s, Value::Number(20));
}

static void DeclareEmptyList()
{
	const std::string s =
		"var a: num[] = []\n"
		"return -8";
	Run(s, Value::Number(-8.0));
}

static void DeclareNumList()
{
	const std::string s =
		"var a: num[] = [1, 2, 3]\n";
	//	"return a[1]";
	//Run(s, Value::Number(2.0));
	Run(s, Value());
}

static void BasicForTest()
{
	const std::string s =
		"var rc = 0\n"
		"for var i = 0; i < 10; i = i + 1 {\n"
		"	rc = rc + 2\n"
		"}\n"
		"return rc";
	Run(s, Value::Number(20));
}

static void BasicForTestNoInit()
{
	const std::string s =
		"var rc = 0\n"
		"var i = 0\n"
		"for ; i < 10; i = i + 1 {\n"
		"	rc = rc + 2\n"
		"}\n"
		"return rc";
	Run(s, Value::Number(20));
}

static void BasicForTestNoDecl()
{
	const std::string s =
		"var rc = 0\n"
		"var i = 0\n"
		"for i = 1; i < 10 + 1; i = i + 1 {\n"
		"	rc = rc + 2\n"
		"}\n"
		"return rc";
	Run(s, Value::Number(20));
}

static void SimpleFFIClock()
{
	const std::string s =
		"print clock()\n"
		"var a: num = clock()\n"
		"var b: num = clock()\n"
		"print(a, b, b-a)\n"
		"return b >= a\n";
	Run(s, Value::Boolean(true));
}

void LangTest()
{
	RUN_TEST(SimplePrint());
	RUN_TEST(SimpleReturn());
	RUN_TEST(SimpleError());
	RUN_TEST(OnePlusTwo());
	RUN_TEST(OneMinusTwo());
	RUN_TEST(TestParen());
	RUN_TEST(TestVarDuck());
	RUN_TEST(TestVarDecl());
	RUN_TEST(TestDuckFail());
	RUN_TEST(TestHelloWorldVar());
	RUN_TEST(AssignVar());
	RUN_TEST(AssignVarPlusOne());
	RUN_TEST(NoRedeclare());
	RUN_TEST(ScopeTest());
	RUN_TEST(AssignmentExpressions());
	RUN_TEST(ShortenedAssignmentExpressions());
	RUN_TEST(BasicIfTest());
	RUN_TEST(BlockTest());
	RUN_TEST(SimpleParenTest());
	RUN_TEST(ParenTest());
	RUN_TEST(BadVarSyntax());
	RUN_TEST(LogicalOR());
	RUN_TEST(LogicalAND());
	RUN_TEST(BasicWhile());
	RUN_TEST(BasicForTest());
	RUN_TEST(BasicForTestNoInit());
	RUN_TEST(BasicForTestNoDecl());
	RUN_TEST(SimpleFFIClock());

#if false
	// note this: https://github.com/munificent/craftinginterpreters/blob/master/note/answers/chapter13_inheritance/3.md
	// come back to this: https://calebschoepp.com/blog/2020/adding-a-list-data-type-to-lox/
	RUN_TEST(DeclareEmptyList());
	RUN_TEST(DeclareNumList());
#endif
}