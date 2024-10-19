#include "machine.h"

#include <fmt/core.h>
#include <assert.h>

static const char* gOpCodeNames[static_cast<int>(OpCode::count)] = {
	"NO_OP",
	"PUSH",
	"POP",
	"ADD",
	"SUB",
	"MUL",
	"DIV",
	"DEFINE_GLOBAL",
	"DEFINE_LOCAL",
	"LOAD",
	"STORE",
	"PUSH_SCOPE",
	"POP_SCOPE",
};

Machine::Machine()
{
}

bool Machine::verifyBinaryNumberOp(const char* op)
{
	if (stack.size() < 2) {
		setErrorMessage(fmt::format("{}: stack underflow", op));
		return false;
	}
	if (stack.back().type != Type::tNumber) {
		setErrorMessage(fmt::format("{}: expected 'number' at stack -1", op));
		return false;
	}
	if (stack[stack.size() - 2].type != Type::tNumber) {
		setErrorMessage(fmt::format("{}: expected 'number' at stack -2", op));
		return false;
	}
	return true;
}

void Machine::binaryNumberOp(OpCode opCode)
{
	if (!verifyBinaryNumberOp(gOpCodeNames[(int)opCode])) return;

	double rhs = stack.back().vNumber;
	stack.pop_back();
	double lhs = stack.back().vNumber;
	stack.pop_back();

	switch (opCode)
	{
	case OpCode::ADD:	stack.push_back(Value(lhs + rhs));	break;
	case OpCode::SUB:	stack.push_back(Value(lhs - rhs));	break;
	case OpCode::MUL:	stack.push_back(Value(lhs * rhs));	break;
	case OpCode::DIV:	stack.push_back(Value(lhs / rhs));	break;
	default:
		setErrorMessage(fmt::format("Unknown opcode when doing binary number operation: {}", (int)opCode));
	}
}

void Machine::negate()
{

}

void Machine::defineGlobal()
{
	size_t s = stack.size();
	if (s < 2) {
		setErrorMessage("DEFINE_GLOBAL: stack underflow");
		return;
	}

	if (stack[s - 2].type != Type::tString) {
		setErrorMessage("DEFINE_GLOBAL: expected 'string' at stack -2");
		return;
	}
	if (stack[s - 1].type == Type::tNone) {
		setErrorMessage("DEFINE_GLOBAL: expected value at stack -1");
		return;
	}

	std::string key = std::move(*(stack[s - 2].vString));
	Value v = std::move(stack[s - 1]);
	stack.pop_back();
	stack.pop_back();

	if (globalVars.find(key) != globalVars.end()) {
		setErrorMessage(fmt::format("DEFINE_GLOBAL: key '{}' already exists", key));
		return;
	}
	globalVars[key] = v;
}


void Machine::defineLocal()
{
	size_t s = stack.size();
	if (s < 2) {
		setErrorMessage("DEFINE_LOCAL: stack underflow");
		return;
	}
	if (stack[s - 2].type != Type::tString) {
		setErrorMessage("DEFINE_LOCAL: expected 'string' at stack -2");
		return;
	}
	if (stack[s - 1].type == Type::tNone) {
		setErrorMessage("DEFINE_LOCAL: expected value at stack -1");
		return;
	}

	std::string key = std::move(*(stack[s - 2].vString));
	Value v = std::move(stack[s - 1]);
	stack.pop_back();
	stack.pop_back();

	const auto it = std::find_if(localVars.begin(), localVars.end(), [&](const LocalVar& lv) {
		return lv.key == key; 
		});

	if (it != localVars.end()) {
		// Note this means that a local variable can not shadow a higher local.
		// It can shadow a global.
		setErrorMessage(fmt::format("DEFINE_LOCAL: key '{}' already exists", key));
		return;
	}

	localVars.push_back({ scopeDepth, key, v });
}

void Machine::load()
{
	size_t s = stack.size();
	if (s < 1) {
		setErrorMessage("LOAD: stack underflow");
		return;
	}
	if (stack[s - 1].type != Type::tString) {
		setErrorMessage("LOAD: expected 'string' at stack -1");
		return;
	}
	std::string key = std::move(*(stack[s - 1].vString));
	stack.pop_back();

	const auto localIt = std::find_if(localVars.begin(), localVars.end(), [&](const LocalVar& lv) {
		return lv.key == key;
		});
	
	if (localIt != localVars.end()) {
		// Found in local scope.
		stack.push_back(localIt->value);
		return;
	}

	const auto globalIt = globalVars.find(key);
	if (globalIt != globalVars.end()) {
		// Found in global scope.
		stack.push_back(globalIt->second);
		return;
	}

	setErrorMessage(fmt::format("LOAD: key '{}' not found", key));
}

void Machine::store()
{
	size_t s = stack.size();
	if (s < 2) {
		setErrorMessage("STORE: stack underflow");
		return;
	}
	if (stack[s - 2].type != Type::tString) {
		setErrorMessage("STORE: expected 'string' at stack -2");
		return;
	}
	if (stack[s - 1].type == Type::tNone) {
		setErrorMessage("STORE: expected value at stack -1");
		return;
	}
	std::string key = std::move(*(stack[s - 2].vString));
	Value v = std::move(stack[s - 1]);
	stack.pop_back();
	stack.pop_back();

	const auto localIt = std::find_if(localVars.begin(), localVars.end(), [&](const LocalVar& lv) {
		return lv.key == key;
		});

	if (localIt != localVars.end()) {
		// Found in local scope.
		if (localIt->value.type != v.type) {
			setErrorMessage(fmt::format("STORE: type mismatch for key '{}'", key));
			return;
		}
		localIt->value = v;
		return;
	}

	const auto globalIt = globalVars.find(key);
	if (globalIt != globalVars.end()) {
		// Found in global scope.
		if (globalIt->second.type != v.type) {
			setErrorMessage(fmt::format("STORE: type mismatch for key '{}'", key));
			return;
		}
		globalIt->second = v;
		return;
	}

	setErrorMessage(fmt::format("STORE: key '{}' not found", key));
}

void Machine::pushScope()
{
	scopeDepth++;
}

void Machine::popScope()
{
	scopeDepth--;

	localVars.erase(std::remove_if(localVars.begin(), localVars.end(), [&](const LocalVar& var) {
		return var.depth > scopeDepth;
		}),
		localVars.end());
}

void Machine::print()
{
	size_t s = stack.size();
	if (s < 1) {
		setErrorMessage("PRINT: stack underflow");
		return;
	}
	fmt::print("{}\n", stack.back().toString());
	stack.pop_back();
}

void Machine::execute(const std::vector<Instruction>& instructions, const ConstPool& pool)
{
	execute(instructions.data(), static_cast<int>(instructions.size()), pool);
}

void Machine::execute(const Instruction* instructions, size_t n, const ConstPool& pool)
{
	for(int i=0; i<n; i++)
	{
		if (hasError()) break;

		OpCode opCode = OpCode::NO_OP;
		uint32_t index = 0;
		UnpackOpCode(instructions[i], opCode, index);

		switch (opCode)
		{
		case OpCode::PUSH:
			stack.push_back(pool.get(index));
			break;
		case OpCode::POP:
			stack.pop_back();
			break;
		case OpCode::ADD:
		case OpCode::SUB:
		case OpCode::MUL:
		case OpCode::DIV:
		{
			binaryNumberOp(opCode);
			break;
		}

		case OpCode::NEGATE:
		case OpCode::NOT:
			unaryOp(opCode);
			break;;

		case OpCode::DEFINE_GLOBAL: defineGlobal(); break;
		case OpCode::DEFINE_LOCAL: defineLocal(); break;
		case OpCode::LOAD: 
			load(); 
			break;
		case OpCode::STORE: 
			store(); 
			break;

		case OpCode::PUSH_SCOPE: pushScope(); break;
		case OpCode::POP_SCOPE: popScope(); break;

		case OpCode::PRINT: print(); break;

		default:
			setErrorMessage(fmt::format("Unknown opcode: {}", (int)opCode));
			break;
		}
	}
}

void Machine::dump(const std::vector<Instruction>& instructions, const ConstPool& pool)
{
	fmt::print("Bytecode:\n");
	for (size_t i = 0; i < instructions.size(); i++)
	{
		OpCode opCode = OpCode::NO_OP;
		uint32_t index = 0;
		UnpackOpCode(instructions[i], opCode, index);

		fmt::print("{: >4}: {: <16}", i, gOpCodeNames[(int)opCode]);
		if (opCode == OpCode::PUSH || opCode == OpCode::DEFINE_GLOBAL || opCode == OpCode::DEFINE_LOCAL || opCode == OpCode::LOAD)
		{
			fmt::print("{: >4}: {}\n", index, pool.get(index).toString());
		}
		else
		{
			fmt::print("\n");
		}
	}
}
