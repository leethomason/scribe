#pragma once

#include "error.h"

#include <string>
#include <vector>
#include <map>

enum class PType : uint8_t {
	tNone,		// not nil - used for error and initialization
	tNumber,
	tString,
	tBoolean,
	tClass		// need to add a class type as well
};

enum class Layout : uint8_t {
	tScalar,
	tList,
	tMap
};

class ValueType {
public:
	ValueType() {}
	ValueType(PType p) : pType(p) {}
	ValueType(PType p, Layout s) : pType(p), layout(s) {}

	PType pType = PType::tNone;
	Layout layout = Layout::tScalar;

	std::string pTypeName() const;
	std::string typeName() const;

	static ValueType fromTypeName(const std::string&);

	bool operator==(const ValueType& rhs) const { return rhs.pType == pType && rhs.layout == layout; }
	bool operator!=(const ValueType& rhs) const { return !(*this == rhs); }
};

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
	static Value Default(ValueType valueType);

	std::string toString() const;

	// Oh boy. This is always up to debate.
	bool isTruthy() const;
	bool isFalsey() const { return !isTruthy(); }

	ValueType type;
	union {
		double vNumber;
		std::string* vString;
		bool vBoolean;
		std::vector<Value>* vArray;
		std::map<std::string, Value>* vMap;
	};

private:
	void clear();
	void copy(const Value& rhs);
};
