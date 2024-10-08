#pragma once

#include "value.h"
#include "error.h"
#include "bytecode.h"

#include <assert.h>
#include <stdint.h>

// Types:
// - number
// - string
// - boolean
// - array
// - map


// The virtual machine.
// Given a stack of instructions, executes them.
class Machine
{
public:
	Machine();
	~Machine() = default;

	void execute(const std::vector<Instruction>& instructions, ConstPool& pool);
	void execute(const Instruction* start, size_t n, ConstPool& pool);

	std::vector<Value> stack;
	std::map<std::string, Value> globalVars;
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

	void setErrorMessage(const std::string& message) { 
		errorMessage = message; 
	}

	bool verifyBinaryNumberOp(const char* op);

	// Stack operations.
	// FIXME: these should be public so the machine can be operated programmatically.
	//        The trick is how to handle the ConstPool - it's currently passed in to execute(),
	//        but individual ops need it.
	void binaryNumberOp(OpCode opCode);
	void defineGlobal();
	void defineLocal();
	void load();
	void store();
	void pushScope();
	void popScope();
};