#pragma once

#include "error.h"

#include <string>
#include <vector>
#include <map>

enum class Type {
	tNone,
	tNumber,
	tString,
	tBoolean,
	tArray,
	tMap,
};

struct Value {
	Value() : type(Type::tNone), vNumber(0) {}
	~Value(); // I. destructor

	Value(const Value& rhs); // II. copy constructor
	Value& operator=(const Value& other); // III. copy assignment
	//Value(Value&& other) noexcept; // IV. move constructor
	//Value& operator=(Value&& rhs) noexcept; // V. move assignment
	
	bool operator==(const Value& rhs) const;
	bool operator!=(const Value& rhs) const { return !(*this == rhs); }

	explicit Value(double v) : type(Type::tNumber), vNumber(v) {}
	explicit Value(const std::string& v) : type(Type::tString) {
		vString = new std::string(v);
	}
	// Why is boolean casting so weird?
	explicit Value(Type t, bool v) : type(Type::tBoolean), vBoolean(v) {
		REQUIRE(t == Type::tBoolean);
	}

	std::string toString() const;

	Type type;
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
