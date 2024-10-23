#pragma once

#include "error.h"
#include "value.h"

#include <stdint.h>
#include <vector>

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

	NEGATE,				// 1					result (-x)
	NOT,				// 1					bool result (true or false)

	EQUAL,
	NOT_EQUAL,
	LESS,
	LESS_EQUAL,
	GREATER,
	GREATER_EQUAL,

	// There isn't really a global, just a top-level scope. And that top-level scope is per file.
	DEFINE_GLOBAL,		// key, init-value		0 (global)
	DEFINE_LOCAL,		// key, init-value		0 (local to current scope depth)
	LOAD,				// key                  value
	STORE,				// key, value			0
	PUSH_SCOPE,			// 0					0
	POP_SCOPE,			// 0					0

	PRINT,				// value				0

	count,
};

inline uint32_t PackOpCode(OpCode op, uint32_t poolIndex) {
	REQUIRE(poolIndex < 0x10000);
	return static_cast<uint32_t>(op) | (poolIndex << 16);
}

inline uint32_t PackOpCode(OpCode op) {
	return static_cast<uint32_t>(op);
}

inline void UnpackOpCode(uint32_t a, OpCode& op, uint32_t& index) {
	uint16_t u16 = static_cast<uint16_t>(a & 0xffff);
	REQUIRE(static_cast<uint32_t>(u16) < static_cast<uint32_t>(OpCode::count));
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
			return (uint32_t)std::distance(values.begin(), it);
		}

		values.push_back(value);
		return static_cast<uint32_t>(values.size() - 1);
	}

	const Value& get(uint32_t index) const {
		// FIXME: should be runtime error
		REQUIRE(index < values.size());
		return values[index];
	}
};

using Instruction = uint32_t;
