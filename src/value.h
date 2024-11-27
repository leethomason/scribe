#pragma once

#include "error.h"

#include <string>
#include <vector>
#include <map>

enum class ValueType {
	tNone,
	tNumber,
	tString,
	tBoolean,
	tArray,
	tMap,
	count
};

class ValueType2 {
public:
	enum class Primary : uint8_t {
		tNone,		// not nil - used for error and initialization
		tNumber,
		tString,
		tBoolean,
		tClass		// need to add a class type as well
	};
	enum class Secondary : uint8_t {
		tScalar,
		tList,
		tMap
	};
	Primary primary = Primary::tNone;
	Secondary secondary = Secondary::tScalar;
};

const char* TypeName(ValueType t);

// Currently: tNumber, tString, tBoolean
// tNone for error
ValueType IdentToTypeName(const std::string&);

struct Value {
	Value() : type(ValueType::tNone), vNumber(0) {}
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
		Value val; val.type = ValueType::tNumber; val.vNumber = v; return val;
	}
	static Value String(const std::string& v) {
		Value val; val.type = ValueType::tString; val.vString = new std::string(v); return val;
	}
	static Value Boolean(bool v) {
		Value val; val.type = ValueType::tBoolean; val.vBoolean = v; return val;
	}

	std::string toString() const;

	// Ooh boy. This is always up to debate.
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
	//void move(Value& other);
};
