#include "value.h"

#include <fmt/core.h>
#include <assert.h>

Value Value::Default(ValueType valueType, Heap& heap)
{
	Value v;

	if (valueType.layout == Layout::tScalar) {
		switch (valueType.pType) {
		case PType::tNumber:
			v = Value::Number(0.0);
			break;
		case PType::tBoolean:
			v = Value::Boolean(false);
			break;
		case PType::tString:
			v = Value::String("");
			break;
		default:
			break; // will throw an error.
		}
	}
	else if (valueType.layout == Layout::tList) {
		v.type = valueType;
		HeapObject* obj = nullptr;

		switch (valueType.pType) {
		case PType::tNumber:
			obj = new NumList(0);
			break;
		case PType::tBoolean:
			obj = new BoolList(0);
			break;
		case PType::tString:
			obj = new StrList(0);
			break;
		default:
			break;
		}
		REQUIRE(obj);
		heap.add(obj);
		v.heapPtr.set(obj);
	}
	else if (valueType.layout == Layout::tMap) {
		assert(false); // not yet implemented
	}
	else {
		assert(false); // not yet implemented
	}
	return v;
}


Value::Value(const Value& rhs)
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
	assert(type.layout == Layout::tScalar); // not yet implemented
	assert(rhs.type.layout == Layout::tScalar); // not yet implemented

	switch (type.pType) {
	case PType::tNone: return true;
	case PType::tNumber: return vNumber == rhs.vNumber;
	case PType::tBoolean: return vBoolean == rhs.vBoolean;
	case PType::tString: return *vString == *rhs.vString;
	default:
		assert(false); // not yet implemnted
	}
	return false;
}

Value::~Value()
{
	clear();
}

void Value::clear()
{
	if (type.layout == Layout::tList) {
		heapPtr.clear();
	}
	else if (type.layout == Layout::tMap) {
		assert(false); // not yet implemented
	}

	switch (type.pType) {
	case PType::tNone:
	case PType::tNumber:
	case PType::tBoolean:
		break;
	case PType::tString: delete vString;
		break;
	default:
		REQUIRE(false);
	}
	type = PType::tNone;
	vNumber = 0;
}

void Value::copy(const Value& rhs)
{
	clear();
	assert(type.layout == Layout::tScalar); // not yet implemented

	type = rhs.type;
	switch (type.pType) {
	case PType::tNone:
		vNumber = 0;
		break;
	case PType::tNumber:
		vNumber = rhs.vNumber;
		break;
	case PType::tBoolean:
		vBoolean = rhs.vBoolean;
		break;
	case PType::tString:
		vString = new std::string(*rhs.vString); 
		break;
	default:
		assert(false); // not yet implemented
	}
}

std::string Value::toString() const
{
	assert(type.layout == Layout::tScalar); // not yet implemented

	switch (type.pType) {
	case PType::tNone:
		return "none";
	case PType::tNumber:
		return fmt::format("{}", vNumber);
	case PType::tBoolean:
		return vBoolean ? "true" : "false";
	case PType::tString:
		return *vString;
	default:
		assert(false); // not implemented
	}
	assert(false);
	return "";
}

bool Value::isTruthy() const
{
	assert(type.layout == Layout::tScalar); // not yet implemented

	switch (type.pType) {
	case PType::tNone:
		return false;
	case PType::tNumber:
		return vNumber != 0.0;
	case PType::tBoolean:
		return vBoolean;
	case PType::tString:
		return !vString->empty();
	default:
		assert(false); // not yet implemented
	}
	REQUIRE(false);
	return false;
}

std::string ValueType::pTypeName() const
{
	std::string name;
	switch (pType) {
	case PType::tNone: return "none";
	case PType::tNumber: return "num";
	case PType::tBoolean: return "bool";
	case PType::tString: return "str";
	default:
		REQUIRE(false);
	}
	return name;
}

std::string ValueType::typeName() const
{
	std::string name = pTypeName();

	if (layout == Layout::tList)
		name += "[]";
	else if (layout == Layout::tMap)
		name += "{}";

	assert(!name.empty());
	return name;
}

/*static*/ ValueType ValueType::fromTypeName(const std::string& name)
{
	ValueType type;
	type.layout = Layout::tScalar;

	if (name == "num") {
		type.pType = PType::tNumber;
	}
	else if (name == "bool") {
		type.pType = PType::tBoolean;
	}
	else if (name == "str") {
		type.pType = PType::tString;
	}
	else {
		assert(false); // not yet implemented
	}
	return type;
}
