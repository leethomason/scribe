#include "value.h"

#include <fmt/core.h>
#include <assert.h>

Value Value::Default(ValueType valueType, Heap& heap)
{
	Value v;

	if (valueType.layout == Layout::tScalar) {
		switch (valueType.pType) {
		case PType::tNum:
			v = Value::Number(0.0);
			break;
		case PType::tBool:
			v = Value::Boolean(false);
			break;
		case PType::tStr:
			v = Value::String("");
			break;

		case PType::tFunc:
		default:
			break; // will throw an error.
		}
	}
	else if (valueType.layout == Layout::tList) {
		v.type = valueType;
		HeapObject* obj = nullptr;

		switch (valueType.pType) {
		case PType::tNum:
			obj = new NumList(0);
			break;
		case PType::tBool:
			obj = new BoolList(0);
			break;
		case PType::tStr:
			obj = new StrList(0);
			break;
		case PType::tFunc:
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
	case PType::tNum: return vNumber == rhs.vNumber;
	case PType::tBool: return vBoolean == rhs.vBoolean;
	case PType::tStr: return *vString == *rhs.vString;
	case PType::tFunc: return *vString == *rhs.vString;
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
	case PType::tNum:
	case PType::tBool:
		break;
	case PType::tStr: 
		delete vString;
		break;
	case PType::tFunc:
		delete vString;
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
	case PType::tNum:
		vNumber = rhs.vNumber;
		break;
	case PType::tBool:
		vBoolean = rhs.vBoolean;
		break;
	case PType::tStr:
		vString = new std::string(*rhs.vString); 
		break;
	case PType::tFunc:
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
	case PType::tNum:
		return fmt::format("{}", vNumber);
	case PType::tBool:
		return vBoolean ? "true" : "false";
	case PType::tStr:
		return *vString;
	case PType::tFunc:
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
	case PType::tNum:
		return vNumber != 0.0;
	case PType::tBool:
		return vBoolean;
	case PType::tStr:
		return !vString->empty();
	case PType::tFunc:
		return true;
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
	case PType::tNum: return "num";
	case PType::tBool: return "bool";
	case PType::tStr: return "str";
	case PType::tFunc: return "func";
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
		type.pType = PType::tNum;
	}
	else if (name == "bool") {
		type.pType = PType::tBool;
	}
	else if (name == "str") {
		type.pType = PType::tStr;
	}
	else {
		assert(false); // not yet implemented
	}
	return type;
}
