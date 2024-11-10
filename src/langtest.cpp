#include "langtest.h"
#include "interpreter.h"
#include "test.h"
#include "errorreporting.h"

#include <string>

static void Run(const std::string& s, Value expectedResult = Value(), bool expectedError = false)
{
	Interpreter ip;
	Value r = ip.interpret(s);
	
	if (expectedError) {
		TEST(ErrorReporter::hasError());
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

	ip.interpret(s);
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

	Run(s, Value(), true);
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

static void TestVarDec()
{
	const std::string s =
		"var x: num = 9\n"
		"return x";

	Run(s, Value::Number(9));
}

static void TestDuckFail()
{
	const std::string s = 
		"var x = 4 + 5\n"
		"return x";
	Run(s, Value(), true);
}

void LangTest()
{
	RUN_TEST(SimplePrint());
	RUN_TEST(SimpleReturn());
	RUN_TEST(SimpleError());
	RUN_TEST(OnePlusTwo());
	RUN_TEST(OneMinusTwo());
	RUN_TEST(TestParen());
}