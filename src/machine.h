#pragma once

#include "value.h"

// Types:
// - number
// - string
// - boolean
// - array
// - struct or map?

enum class OpCode
{
	NO_OP,
	PUSH,
	POP,
	ADD,
	SUB,
	MUL,
	DIV,
	count,
};

struct Instruction
{
	OpCode opCode = OpCode::NO_OP;
	Value value;
};

// The virtual machine.
// Given a stack of instructions, executes them.
class Machine
{
public:
	Machine() = default;
	~Machine() = default;

	void execute(const std::vector<Instruction>& instructions);

	std::vector<Value> stack;
	std::string errorMessage;

	bool hasError() const { return !errorMessage.empty(); }

	static void test();

private:
	void setErrorMessage(const std::string& message) { errorMessage = message; }

	bool verifyBinaryNumberOp(const char* op);
	void doBinaryNumberOp(OpCode opCode);
};