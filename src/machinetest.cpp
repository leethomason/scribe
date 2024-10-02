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

// 1 + 2 = 3
void OnePlusTwoIsThree()
{
	std::map<std::string, Value> globals;
	Machine machine;
	std::vector<Instruction> instructions = {
		{ OpCode::PUSH, Value::Number(1.0) },		// lhs
		{ OpCode::PUSH, Value::Number(2.0) },		// rhs
		{ OpCode::ADD, Value() },
	};
	machine.execute(instructions, globals);

	TEST(machine.hasError() == false);
	TEST(machine.stack.size() == 1);
	TEST(machine.stack[0].type == Type::tNumber);
	TEST_FP(machine.stack[0].vNumber, 3.0);
}

// 1 - 2 = -1
void OneMinusTwoIsNegativeOne()
{
	std::map<std::string, Value> globals;
	Machine machine;
	std::vector<Instruction> instructions = {
		{ OpCode::PUSH, Value::Number(1.0) },		// lhs
		{ OpCode::PUSH, Value::Number(2.0) },		// rhs
		{ OpCode::SUB, Value() },
	};
	machine.execute(instructions, globals);

	TEST(machine.hasError() == false);
	TEST(machine.stack.size() == 1);
	TEST(machine.stack[0].type == Type::tNumber);
	TEST_FP(machine.stack[0].vNumber, -1.0);
}

// (1 + 2) * 3 = 9
void OnePlusTwoTimesThreeIsNine()
{
	std::map<std::string, Value> globals;
	Machine machine;
	std::vector<Instruction> instructions = {
		{ OpCode::PUSH, Value::Number(1.0) },		// lhs
		{ OpCode::PUSH, Value::Number(2.0) },		// rhs
		{ OpCode::ADD, Value() },
		{ OpCode::PUSH, Value::Number(3.0) },		// rhs
		{ OpCode::MUL, Value() },
	};
	machine.execute(instructions, globals);

	TEST(machine.hasError() == false);
	TEST(machine.stack.size() == 1);
	TEST(machine.stack[0].type == Type::tNumber);
	TEST_FP(machine.stack[0].vNumber, 9.0);
}

// (1 + 2) / 3 = 1
void OnePlusTwoDivThreeIsOne()
{
	std::map<std::string, Value> globals;
	Machine machine;
	std::vector<Instruction> instructions = {
		{ OpCode::PUSH, Value::Number(1.0) },		// lhs
		{ OpCode::PUSH, Value::Number(2.0) },		// rhs
		{ OpCode::ADD, Value() },
		{ OpCode::PUSH, Value::Number(3.0) },		// rhs
		{ OpCode::DIV, Value() },
	};
	machine.execute(instructions, globals);

	TEST(machine.hasError() == false);
	TEST(machine.stack.size() == 1);
	TEST(machine.stack[0].type == Type::tNumber);
	TEST_FP(machine.stack[0].vNumber, 1.0);
}

// x = 1
// y = 2
// x + y = 3
void XPlusYIsThree()
{
}

void Machine::test()
{
	RUN_TEST(OnePlusTwoIsThree());
	RUN_TEST(OneMinusTwoIsNegativeOne());
	RUN_TEST(OnePlusTwoTimesThreeIsNine());
	RUN_TEST(OnePlusTwoDivThreeIsOne());
}
