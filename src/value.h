#pragma once

#include "error.h"
#include "type.h"
#include "heap.h"

#include <string>
#include <vector>
#include <map>

struct Value {
	Value() : vNumber(0) {}
	~Value(); // I. destructor

	Value(const Value& rhs); // II. copy constructor
	Value& operator=(const Value& other); // III. copy assignment
	//Value(Value&& other) noexcept; // IV. move constructor
	//Value& operator=(Value&& rhs) noexcept; // V. move assignment
	
	bool operator==(const Value& rhs) const;
	bool operator!=(const Value& rhs) const { return !(*this == rhs); }

	// Fooling around with overloaded constructors (especially bool)
	// is a mess. Use constructor functions.

	static Value Number(double v) {
		Value val; val.type.pType = PType::tNumber; val.vNumber = v; return val;
	}
	static Value String(const std::string& v) {
		Value val; val.type.pType = PType::tString; val.vString = new std::string(v); return val;
	}
	static Value Boolean(bool v) {
		Value val; val.type.pType = PType::tBoolean; val.vBoolean = v; return val;
	}
	static Value Default(ValueType valueType, Heap& heap);

	std::string toString() const;

	// Oh boy. This is always up to debate.
	bool isTruthy() const;
	bool isFalsey() const { return !isTruthy(); }

	ValueType type;
	union {
		double vNumber;
		std::string* vString;
		bool vBoolean;
	};
	HeapPtr heapPtr;

private:
	void clear();
	void copy(const Value& rhs);
};
