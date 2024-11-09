#include "langtest.h"
#include "interpreter.h"
#include "test.h"

#include <string>

static void SimpleReturn()
{
	const std::string s =
		"print 13";
	
	Interpreter itpr;
	Value r = itpr.interpret(s);
	TEST(r.type == ValueType::tNumber);
	TEST(r.vNumber == 13);
}

void LangTest()
{
	RUN_TEST(SimpleReturn());
}