#pragma once

#include <string>
#include <stdint.h>

enum class PType : uint8_t {
	tNone,		// not nil - used for error and initialization
	tNum,
	tStr,
	tBool,
	tFunc,
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
