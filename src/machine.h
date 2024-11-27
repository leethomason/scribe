#pragma once

#include "value.h"
#include "error.h"
#include "bytecode.h"

#include <assert.h>
#include <stdint.h>

#if 0
// The virtual machine.
// Given a stack of instructions, executes them.
class Machine
{
public:
	Machine();
	~Machine() = default;

	void execute(const std::vector<Instruction>& instructions, const ConstPool& pool);
	void execute(const Instruction* start, size_t n, const ConstPool& pool);

	std::vector<Value> stack;
	std::map<std::string, Value> globalVars;

	bool hasError() const { return !error.empty(); }
	const std::string& errorMessage() const { return error;}

	void dump(const std::vector<Instruction>& instructions, const ConstPool& pool);

	static void test();

private:
	std::string error;
	struct LocalVar {
		int depth = 0;
		std::string key;
		Value value;
	};
	std::vector<LocalVar> localVars;
	int scopeDepth = 0;

	void setErrorMessage(const std::string& message) { 
		error = message; 
	}

	bool verifyUnderflow(const std::string& ctx, int n);
	bool verifyTypes(const std::string& ctx, const std::vector<ValueType>& types);	// checks underflow as well

	void popStack(int n = 1);

	// Warning: unchecked!
	Value& getStack(int i) { 
		assert(i > 0 && i <= stack.size());
		return stack[stack.size() - i]; 
	}

	// Stack operations.
	// FIXME: these should be public so the machine can be operated programmatically.
	//        The trick is how to handle the ConstPool - it's currently passed in to execute(),
	//        but individual ops need it.
	void binaryOp(OpCode opCode);

	void equal(OpCode op);			// any type
	void compare(OpCode opCode);	// numbers only

	void negative();
	void notOp();
	void defineGlobal();
	void defineLocal();
	void load();
	void store();
	void pushScope();
	void popScope();
	void print();
};
#endif
