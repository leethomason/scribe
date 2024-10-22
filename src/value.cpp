#include "value.h"

#include <fmt/core.h>
#include <assert.h>

const char* TypeName(Type t)
{
	REQUIRE((int)t >= 0 && (int)t < (int)Type::count);

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

Value::Value(const Value& rhs) : type(Type::tNone)
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
	case Type::tNone: return true;
	case Type::tNumber: return vNumber == rhs.vNumber;
	case Type::tBoolean: return vBoolean == rhs.vBoolean;
	case Type::tString: return *vString == *rhs.vString;
	case Type::tArray: return *vArray == *rhs.vArray;
	case Type::tMap: return *vMap == *rhs.vMap;
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
	case Type::tNone:
	case Type::tNumber:
	case Type::tBoolean:
		break;
	case Type::tString: delete vString; break;
	case Type::tArray: delete vArray; break;
	case Type::tMap: delete vMap; break;
	}
	type = Type::tNone;
	vNumber = 0;
}

void Value::copy(const Value& rhs)
{
	clear();
	type = rhs.type;
	switch (type) {
	case Type::tNone:
		vNumber = 0;
		break;
	case Type::tNumber:
		vNumber = rhs.vNumber;
		break;
	case Type::tBoolean:
		vBoolean = rhs.vBoolean;
		break;
	case Type::tString: 
		vString = new std::string(*rhs.vString); 
		break;
	case Type::tArray: 
		vArray = new std::vector<Value>(*rhs.vArray); 
		break;
	case Type::tMap: 
		vMap = new std::map<std::string, Value>(*rhs.vMap); 
		break;
	}
}

std::string Value::toString() const
{
	switch (type) {
	case Type::tNone:
		return "none";
	case Type::tNumber:
		return fmt::format("{}", vNumber);
	case Type::tBoolean:
		return vBoolean ? "true" : "false";
	case Type::tString:
		return *vString;
	case Type::tArray:
		return "[]";
	case Type::tMap:
		return "{}";
	}
	assert(false);
	return "";
}

bool Value::isTruthy() const
{
	switch (type) {
	case Type::tNone:
		return false;
	case Type::tNumber:
		return vNumber != 0;
	case Type::tBoolean:
		return vBoolean;
	case Type::tString:
		return !vString->empty();
	case Type::tArray:
		return !vArray->empty();
	case Type::tMap:
		return !vMap->empty();
	}
	REQUIRE(false);
	return false;
}