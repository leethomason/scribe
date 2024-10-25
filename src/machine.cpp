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

bool Machine::verifyUnderflow(const std::string& ctx, int n)
{
	if (stack.size() < n) {
		setErrorMessage(fmt::format("{}: stack underflow", ctx));
		return false;
	}
	return true;
}

bool Machine::verifyTypes(const std::string& ctx, const std::vector<Type>& types)
{
	REQUIRE(types.size() > 0);

	if (!verifyUnderflow(ctx, (int)types.size())) return false;
	for (size_t i = 0; i < types.size(); i++) {
		Type type = getStack(1).type;
		if (type != types[i]) {
			setErrorMessage(fmt::format("{}: expected '{}' at stack -{}", ctx, TypeName(type), i+1));
			return false;
		}
	}
	return true;
}

void Machine::popStack(int n)
{
	REQUIRE(n >= 0);
	REQUIRE(stack.size() >= n);
	stack.resize(stack.size() - n);
}

void Machine::binaryOp(OpCode opCode)
{
	bool stringAdd = stack.size() > 1 && getStack(1).type == Type::tString;
	if (stringAdd) {
		if (!verifyTypes("ADD concat", {Type::tString, Type::tString})) return;

		const std::string& lhs = *getStack(2).vString;
		const std::string& rhs = *getStack(1).vString;
		std::string result = lhs + rhs;
		popStack(2);
		stack.push_back(Value::String(result));
	}
	else {
		if (!verifyTypes(gOpCodeNames[(int)opCode], {Type::tNumber, Type::tNumber})) return;

		double rhs = getStack(1).vNumber;
		double lhs = getStack(2).vNumber;
		popStack(2);

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
}

void Machine::equal(OpCode op)
{
	REQUIRE(op == OpCode::EQUAL || op == OpCode::NOT_EQUAL);
	const std::string& opName = op == OpCode::EQUAL ? "EQUAL" : "NOT_EQUAL";
	if (!verifyUnderflow(opName, 2)) return;
	const Value& rhs = getStack(1);
	const Value& lhs = getStack(2);
	if (rhs.type != lhs.type) {
		setErrorMessage(fmt::format("{}: type mismatch: {} != {}", opName, TypeName(lhs.type), TypeName(rhs.type)));
		return;
	}
	bool result = rhs == lhs;
	if (op == OpCode::NOT_EQUAL) result = !result;

	popStack(2);
	stack.push_back(Value::Boolean(result));
}

void Machine::compare(OpCode opCode)
{
	REQUIRE(opCode == OpCode::LESS || opCode == OpCode::LESS_EQUAL || opCode == OpCode::GREATER || opCode == OpCode::GREATER_EQUAL);
	const std::string& opName = gOpCodeNames[(int)opCode];
	if (!verifyTypes(opName, { Type::tNumber, Type::tNumber })) return;

	double rhs = getStack(1).vNumber;
	double lhs = getStack(2).vNumber;
	popStack(2);

	bool result = false;
	switch (opCode)
	{
	case OpCode::LESS:			result = lhs < rhs; break;
	case OpCode::LESS_EQUAL:	result = lhs <= rhs; break;
	case OpCode::GREATER:		result = lhs > rhs; break;
	case OpCode::GREATER_EQUAL:	result = lhs >= rhs; break;
	default:
		setErrorMessage(fmt::format("Unknown opcode when doing comparison: {}", (int)opCode));
	}
	stack.push_back(Value::Boolean(result));
}

void Machine::negative()
{
	if (!verifyTypes("NEGATE", {Type::tNumber})) return;
	double x = getStack(1).vNumber;
	//popStack(1);
	//stack.push_back(Value::Number(-x));
	getStack(1) = Value::Number(-x);
}

void Machine::notOp()
{
	if (!verifyUnderflow("NOT", 1)) return;
	bool x = getStack(1).isTruthy();
	//popStack(1);
	//stack.push_back(Value::Boolean(!x));
	getStack(1) = Value::Boolean(!x);
}

void Machine::defineGlobal()
{
	size_t s = stack.size();
	if (s < 2) {
		setErrorMessage("DEFINE_GLOBAL: stack underflow");
		return;
	}

	if (getStack(2).type != Type::tString) {
		setErrorMessage("DEFINE_GLOBAL: expected 'string' at stack -2");
		return;
	}
	if (getStack(1).type == Type::tNone) {
		setErrorMessage("DEFINE_GLOBAL: expected value at stack -1");
		return;
	}

	std::string key = std::move(*(getStack(2).vString));
	Value v = std::move(getStack(1));
	popStack(2);

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
	if (getStack(2).type != Type::tString) {
		setErrorMessage("DEFINE_LOCAL: expected 'string' at stack -2");
		return;
	}
	if (getStack(1).type == Type::tNone) {
		setErrorMessage("DEFINE_LOCAL: expected value at stack -1");
		return;
	}

	std::string key = std::move(*(getStack(2).vString));
	Value v = getStack(1);
	popStack(2);

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
	if (getStack(1).type != Type::tString) {
		setErrorMessage("LOAD: expected 'string' at stack -1");
		return;
	}
	std::string key = std::move(*(getStack(1).vString));
	popStack();

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
	if (getStack(2).type != Type::tString) {
		setErrorMessage("STORE: expected 'string' at stack -2");
		return;
	}
	if (getStack(1).type == Type::tNone) {
		setErrorMessage("STORE: expected value at stack -1");
		return;
	}
	std::string key = std::move(*(getStack(2).vString));
	Value v = std::move(getStack(1));
	popStack(2);

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
	fmt::print("{}\n", getStack(1).toString());
	popStack();
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
			popStack();
			break;
		case OpCode::ADD:
		case OpCode::SUB:
		case OpCode::MUL:
		case OpCode::DIV:
		{
			binaryOp(opCode);
			break;
		}

		case OpCode::NOT: notOp(); break;
		case OpCode::NEGATE: negative(); break;
		case OpCode::DEFINE_GLOBAL: defineGlobal(); break;
		case OpCode::DEFINE_LOCAL: defineLocal(); break;
		case OpCode::LOAD: load(); break;
		case OpCode::STORE: store(); break;

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
