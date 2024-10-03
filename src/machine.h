#pragma once

#include "value.h"
#include <assert.h>
#include <stdint.h>

// Types:
// - number
// - string
// - boolean
// - array
// - map

enum class OpCode : uint16_t
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

inline uint32_t PackOpCode(OpCode op, uint32_t poolIndex) {
	assert(poolIndex < 0x10000);
	return static_cast<uint32_t>(op) | (poolIndex << 16);
}

inline uint32_t PackOpCode(OpCode op) {
	return static_cast<uint32_t>(op);
}

inline void UnpackOpCode(uint32_t a, OpCode& op, uint32_t& index) {
	uint16_t u16 = static_cast<uint16_t>(a & 0xffff);
	assert(static_cast<uint32_t>(u16) < static_cast<uint32_t>(OpCode::count));
	op = static_cast<OpCode>(u16);
	index = a >> 16;
}

struct ConstPool
{
	ConstPool() {
		// Useful, and this way there is always a 0 index.
		values.push_back(Value::Number(0.0));
	}

	std::vector<Value> values;

	uint32_t add(const Value& value) {
		auto it = std::find(values.begin(), values.end(), value);
		if (it != values.end()) {
			return (uint32_t) std::distance(values.begin(), it);
		}

		values.push_back(value);
		return static_cast<uint32_t>(values.size() - 1);
	}

	const Value& get(uint32_t index) const {
		assert(index < values.size());
		return values[index];
	}
};

using Instruction = uint32_t;

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