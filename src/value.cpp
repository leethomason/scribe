#include "value.h"

#include <fmt/core.h>
#include <assert.h>

const char* TypeName(ValueType t)
{
	REQUIRE((int)t >= 0 && (int)t < (int)ValueType::count);

	static const char* names[] = {
		"none",
		"number",
		"string",
		"boolean",
		"array",
		"map",
	};
	return names[static_cast<int>(t)];
}

Value::Value(const Value& rhs) : type(ValueType::tNone)
{
	copy(rhs);
}

Value& Value::operator=(const Value& rhs)
{
	if (this == &rhs) return *this;
	copy(rhs);
	return *this;
}

bool Value::operator==(const Value& rhs) const
{
	if (type != rhs.type) return false;

	switch (type) {
	case ValueType::tNone: return true;
	case ValueType::tNumber: return vNumber == rhs.vNumber;
	case ValueType::tBoolean: return vBoolean == rhs.vBoolean;
	case ValueType::tString: return *vString == *rhs.vString;
	case ValueType::tArray: return *vArray == *rhs.vArray;
	case ValueType::tMap: return *vMap == *rhs.vMap;
	}
	return false;
}

Value::~Value()
{
	clear();
}

#if 0
Value::Value(Value&& other) noexcept
{
	move(other);
}

Value& Value::operator=(Value&& rhs) noexcept
{
	if (this == &rhs) return *this;
	move(rhs);
	return *this;
}

void Value::move(Value& other)
{
	clear();
	type = other.type;
	other.type = Type::tNone;

	switch (type) {
	case Type::tNone: 
		vNumber = 0; 
		break;
	case Type::tNumber:
		vNumber = other.vNumber;
		break;
	case Type::tBoolean:
		vBoolean = other.vBoolean;
		break;
	case Type::tString:
		vString = other.vString; 
		other.vString = nullptr;
		break;
	case Type::tArray:
		vArray = other.vArray;
		other.vArray = nullptr;
		break;
	case Type::tMap:
		vMap = other.vMap;
		other.vMap = nullptr;
		break;
	}
}
#endif

void Value::clear()
{
	switch (type) {
	case ValueType::tNone:
	case ValueType::tNumber:
	case ValueType::tBoolean:
		break;
	case ValueType::tString: delete vString; break;
	case ValueType::tArray: delete vArray; break;
	case ValueType::tMap: delete vMap; break;
	}
	type = ValueType::tNone;
	vNumber = 0;
}

void Value::copy(const Value& rhs)
{
	clear();
	type = rhs.type;
	switch (type) {
	case ValueType::tNone:
		vNumber = 0;
		break;
	case ValueType::tNumber:
		vNumber = rhs.vNumber;
		break;
	case ValueType::tBoolean:
		vBoolean = rhs.vBoolean;
		break;
	case ValueType::tString: 
		vString = new std::string(*rhs.vString); 
		break;
	case ValueType::tArray: 
		vArray = new std::vector<Value>(*rhs.vArray); 
		break;
	case ValueType::tMap: 
		vMap = new std::map<std::string, Value>(*rhs.vMap); 
		break;
	}
}

std::string Value::toString() const
{
	switch (type) {
	case ValueType::tNone:
		return "none";
	case ValueType::tNumber:
		return fmt::format("{}", vNumber);
	case ValueType::tBoolean:
		return vBoolean ? "true" : "false";
	case ValueType::tString:
		return *vString;
	case ValueType::tArray:
		return "[]";
	case ValueType::tMap:
		return "{}";
	}
	assert(false);
	return "";
}

bool Value::isTruthy() const
{
	switch (type) {
	case ValueType::tNone:
		return false;
	case ValueType::tNumber:
		return vNumber != 0;
	case ValueType::tBoolean:
		return vBoolean;
	case ValueType::tString:
		return !vString->empty();
	case ValueType::tArray:
		return !vArray->empty();
	case ValueType::tMap:
		return !vMap->empty();
	}
	REQUIRE(false);
	return false;
}