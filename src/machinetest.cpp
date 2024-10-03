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

	REQUIRE(def == OpCode::DEFINE_GLOBAL || def == OpCode::DEFINE_LOCAL);

	std::vector<Instruction> instructions = {
		PackOpCode(OpCode::PUSH, x),			// "x"
		PackOpCode(OpCode::PUSH, one),			// 1
		PackOpCode(def),						// 1 stored to var "x"
		PackOpCode(OpCode::PUSH, y),
		PackOpCode(OpCode::PUSH, two),
		PackOpCode(def),						// 2 stored to var "y"
		PackOpCode(OpCode::PUSH, x),			// "x"
		PackOpCode(OpCode::LOAD),				// -> 1
		PackOpCode(OpCode::PUSH, y),			// "y"
		PackOpCode(OpCode::LOAD, y),			// -> 2
		PackOpCode(OpCode::ADD),
	};

	machine.execute(instructions.data(), 3, pool);
	TEST(machine.hasError() == false);
	TEST(machine.stack.size() == 0);

	machine.execute(instructions.data() + 3, 3, pool);
	TEST(machine.hasError() == false);
	TEST(machine.stack.size() == 0);

	machine.execute(instructions.data() + 6, instructions.size() - 6, pool);
	TEST(machine.hasError() == false);
	TEST(machine.stack.size() == 1);
	TEST(machine.stack[0].type == Type::tNumber);
	TEST_FP(machine.stack[0].vNumber, 3.0);
}

// a = "Hello";
// a += " World";

// a = true/false
// x = 0
// if (a) 
//   x = 1
// else
//   x = 2

// a = [1, 2, 3]
// a[1]

void Machine::test()
{
	RUN_TEST(OnePlusTwoIsThree());
	RUN_TEST(OneMinusTwoIsNegativeOne());
	RUN_TEST(OnePlusTwoTimesThreeIsNine());
	RUN_TEST(OnePlusTwoDivThreeIsOne());
	RUN_TEST(XPlusYIsThree(OpCode::DEFINE_GLOBAL));
	RUN_TEST(XPlusYIsThree(OpCode::DEFINE_LOCAL));
}
