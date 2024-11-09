#include "langtest.h"
#include "interpreter.h"
#include "test.h"

#include <string>

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
	
	Interpreter ip;
	Value r = ip.interpret(s);
	TEST(r.type == ValueType::tNumber);
	TEST(r.vNumber == 13);
}

static void OnePlusTwo()
{
	const std::string s = 
			"return 1 + 2";

	Interpreter ip;
	Value r = ip.interpret(s);
	TEST(r.type == ValueType::tNumber);
	TEST(r.vNumber == 3);
}

static void OneMinusTwo()
{
	const std::string s =
		"return 1 - 2";

	Interpreter ip;
	Value r = ip.interpret(s);
	TEST(r.type == ValueType::tNumber);
	TEST(r.vNumber == -1);
}
void LangTest()
{
	RUN_TEST(SimplePrint());
	RUN_TEST(SimpleReturn());
	RUN_TEST(OnePlusTwo());
	RUN_TEST(OneMinusTwo());
}