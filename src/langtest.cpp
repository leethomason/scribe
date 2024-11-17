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

static void SimplePrint()
{
	const std::string s =
		"print 13";

	Interpreter ip;
	std::string out;
	ip.setOutput(out);

	ip.interpret(s, "langtest");
	TEST(out == "13\n");
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
		"return 13;";

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
		"var x: number = 9\n"
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

// Need 'if' to turn on...
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
}