#include "machine.h"

#include <fmt/core.h>

static const char* gOpCodeNames[static_cast<int>(OpCode::count)] = {
	"NO_OP",
	"PUSH",
	"POP",
	"ADD",
	"SUB",
	"MUL",
	"DIV",
};

bool Machine::verifyBinaryNumberOp(const char* op)
{
	if (stack.size() < 2) {
		setErrorMessage(fmt::format("{}: stack underflow", op));
		return false;
	}
	if (stack.back().type != Type::tNumber) {
		setErrorMessage(fmt::format("{}: expected 'number' at stack - 1", op));
		return false;
	}
	if (stack[stack.size() - 2].type != Type::tNumber) {
		setErrorMessage(fmt::format("{}: expected 'number' at stack - 2", op));
		return false;
	}
	return true;
}

void Machine::doBinaryNumberOp(OpCode opCode)
{
	if (!verifyBinaryNumberOp(gOpCodeNames[(int)opCode])) return;

	double rhs = stack.back().vNumber;
	stack.pop_back();
	double lhs = stack.back().vNumber;
	stack.pop_back();

	switch (opCode)
	{
	case OpCode::ADD:	stack.push_back(Value::Number(lhs + rhs));	break;
	case OpCode::SUB:	stack.push_back(Value::Number(lhs - rhs));	break;
	case OpCode::MUL:	stack.push_back(Value::Number(lhs * rhs));	break;
	case OpCode::DIV:	stack.push_back(Value::Number(lhs / rhs));	break;
	default:
		setErrorMessage(fmt::format("Unknown opcode when doing binary number operation: {}", (int)opCode));
	}
}


void Machine::execute(const std::vector<Instruction>& instructions)
{
	for (const auto& instruction : instructions)
	{
		switch (instruction.opCode)
		{
		case OpCode::PUSH:
			stack.push_back(instruction.value);
			break;
		case OpCode::POP:
			stack.pop_back();
			break;
		case OpCode::ADD:
		case OpCode::SUB:
		case OpCode::MUL:
		case OpCode::DIV:
		{
			doBinaryNumberOp(instruction.opCode);
			break;
		}
		default:
			setErrorMessage(fmt::format("Unknown opcode: {}", (int)instruction.opCode));
			break;
		}
	}
}
