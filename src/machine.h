#pragma once

#include "value.h"

// Types:
// - number
// - string
// - boolean
// - array
// - map

enum class OpCode
{
	//						pop					push
	NO_OP,

	PUSH,				//  0					value	
	POP,				//  1					0

	ADD,				// lhs, rhs				result
	SUB,
	MUL,
	DIV,

	// There isn't really a global, just a top-level scope. And that top-level scope is per file.
	DEFINE_GLOBAL,		// key, init-value		0 (global)
	DEFINE_LOCAL,		// key, init-value		0 (local to current scope depth)
	LOAD,				// key                  value
	STORE,				// key, value			0
	PUSH_SCOPE,			// 0					0
	POP_SCOPE,			// 0					0

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
	Machine();
	~Machine() = default;

	void execute(const std::vector<Instruction>& instructions, std::map<std::string, Value>& globalVars);

	std::vector<Value> stack;
	std::string errorMessage;

	bool hasError() const { return !errorMessage.empty(); }

	static void test();

private:
	struct LocalVar {
		int depth = 0;
		std::string key;
		Value value;
	};
	std::vector<LocalVar> localVars;
	int scopeDepth = 0;
	std::map<std::string, Value>* globalVars = nullptr;

	void setErrorMessage(const std::string& message) { errorMessage = message; }

	bool verifyBinaryNumberOp(const char* op);
	void doBinaryNumberOp(OpCode opCode);
	
	void doDefineGlobal();
	void doDefineLocal();
	void doLoad();
	void doStore();
	void doPushScope();
	void doPopScope();
};