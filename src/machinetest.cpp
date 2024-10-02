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
	ConstPool pool;
	const uint32_t one = pool.add(Value::Number(1.0));
	const uint32_t two = pool.add(Value::Number(2.0));

	Machine machine;
	std::vector<Instruction> instructions = {
		PackOpCode(OpCode::PUSH, one),		// lhs
		PackOpCode(OpCode::PUSH, two),		// rhs
		PackOpCode(OpCode::ADD),
	};
	machine.execute(instructions, pool);

	TEST(machine.hasError() == false);
	TEST(machine.stack.size() == 1);
	TEST(machine.stack[0].type == Type::tNumber);
	TEST_FP(machine.stack[0].vNumber, 3.0);
}

// 1 - 2 = -1
void OneMinusTwoIsNegativeOne()
{
	ConstPool pool;
	Machine machine;

	const uint32_t one = pool.add(Value::Number(1.0));
	const uint32_t two = pool.add(Value::Number(2.0));

	std::vector<Instruction> instructions = {
		PackOpCode(OpCode::PUSH, one ),		// lhs
		PackOpCode(OpCode::PUSH, two ),		// rhs
		PackOpCode(OpCode::SUB),
	};
	machine.execute(instructions, pool);

	TEST(machine.hasError() == false);
	TEST(machine.stack.size() == 1);
	TEST(machine.stack[0].type == Type::tNumber);
	TEST_FP(machine.stack[0].vNumber, -1.0);
}

// (1 + 2) * 3 = 9
void OnePlusTwoTimesThreeIsNine()
{
	ConstPool pool;
	Machine machine;

	const uint32_t one = pool.add(Value::Number(1.0));
	const uint32_t two = pool.add(Value::Number(2.0));
	const uint32_t three = pool.add(Value::Number(3.0));

	std::vector<Instruction> instructions = {
		PackOpCode(OpCode::PUSH, one),		// lhs
		PackOpCode(OpCode::PUSH, two),		// rhs
		PackOpCode(OpCode::ADD),
		PackOpCode(OpCode::PUSH, three),	// rhs
		PackOpCode(OpCode::MUL),
	};
	machine.execute(instructions, pool);

	TEST(machine.hasError() == false);
	TEST(machine.stack.size() == 1);
	TEST(machine.stack[0].type == Type::tNumber);
	TEST_FP(machine.stack[0].vNumber, 9.0);
}

// (1 + 2) / 3 = 1
void OnePlusTwoDivThreeIsOne()
{
	ConstPool pool;
	const uint32_t one = pool.add(Value::Number(1.0));
	const uint32_t two = pool.add(Value::Number(2.0));
	const uint32_t three = pool.add(Value::Number(3.0));

	Machine machine;
	std::vector<Instruction> instructions = {
		PackOpCode(OpCode::PUSH, one),		// lhs
		PackOpCode(OpCode::PUSH, two),		// rhs
		PackOpCode(OpCode::ADD),
		PackOpCode(OpCode::PUSH, three),	// rhs
		PackOpCode(OpCode::DIV),
	};
	machine.execute(instructions, pool);

	TEST(machine.hasError() == false);
	TEST(machine.stack.size() == 1);
	TEST(machine.stack[0].type == Type::tNumber);
	TEST_FP(machine.stack[0].vNumber, 1.0);
}

// x = 1
// y = 2
// x + y = 3
void XPlusYIsThree(OpCode def)
{
	ConstPool pool;
	Machine machine;

	const uint32_t x = pool.add(Value::String("x"));
	const uint32_t y = pool.add(Value::String("y"));
	const uint32_t one = pool.add(Value::Number(1.0));
	const uint32_t two = pool.add(Value::Number(2.0));

	assert(def == OpCode::DEFINE_GLOBAL || def == OpCode::DEFINE_LOCAL);

	std::vector<Instruction> instructions = {
		PackOpCode(OpCode::PUSH, x),
		PackOpCode(OpCode::PUSH, one),
		PackOpCode(def),
		PackOpCode(OpCode::PUSH, y),
		PackOpCode(OpCode::PUSH, two),
		PackOpCode(def),
		PackOpCode(OpCode::LOAD, x),
		PackOpCode(OpCode::LOAD, y),
		PackOpCode(OpCode::ADD),
	};
	machine.execute(instructions, pool);

	TEST(machine.hasError() == false);
	TEST(machine.stack.size() == 1);
	TEST(machine.stack[0].type == Type::tNumber);
	TEST_FP(machine.stack[0].vNumber, 3.0);
}

void Machine::test()
{
	RUN_TEST(OnePlusTwoIsThree());
	RUN_TEST(OneMinusTwoIsNegativeOne());
	RUN_TEST(OnePlusTwoTimesThreeIsNine());
	RUN_TEST(OnePlusTwoDivThreeIsOne());
	RUN_TEST(XPlusYIsThree(OpCode::DEFINE_GLOBAL));
	RUN_TEST(XPlusYIsThree(OpCode::DEFINE_LOCAL));
}
