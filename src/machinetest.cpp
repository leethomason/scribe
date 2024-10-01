#include "machine.h"
#include "fmt/core.h"

#include <assert.h>

#define TEST(x)                                                 \
	if (!(x)) {	                                                \
		fmt::print("Test failure line {} in {}\n", __LINE__, __FILE__);\
        assert(false);                                          \
	}															\

#define TEST_FP(x, y)                                           \
	if (fabs(x - y) > 0.0001) {	                                \
		fmt::print("Test failure line {} in {}\n", __LINE__, __FILE__);\
        assert(false);                                          \
	}                                                           \

#define RUN_TEST(test) fmt::print("Test: {}\n", #test); test

void OnePlusTwoIsThree()
{
	Machine machine;
	std::vector<Instruction> instructions = {
		{ OpCode::PUSH, Value::Number(1.0) },		// lhs
		{ OpCode::PUSH, Value::Number(2.0) },		// rhs
		{ OpCode::ADD, Value() },
	};
	machine.execute(instructions);

	TEST(machine.hasError() == false);
	TEST(machine.stack.size() == 1);
	TEST(machine.stack[0].type == Type::tNumber);
	TEST_FP(machine.stack[0].vNumber, 3.0);
}


void Machine::test()
{
	RUN_TEST(OnePlusTwoIsThree());
}
