#pragma once

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
	Value(Value&& other) noexcept; // IV. move constructor
	Value& operator=(Value&& rhs) noexcept; // V. move assignment
	
	bool operator==(const Value& rhs) const;
	bool operator!=(const Value& rhs) const { return !(*this == rhs); }

	static Value Number(double a) {
		Value v;
		v.type = Type::tNumber;
		v.vNumber = a;
		return v;
	}

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
	void move(Value& other);
};
