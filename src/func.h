#pragma once

#include "value.h"

#include <vector>
#include <map>

class Interpreter;

class Callable
{
public:
	virtual void call(Interpreter& inter) = 0;
};

class FFIHandler
{
public:
	virtual Value call(const std::string& name, 
		const std::vector<Value>& args, 
		ValueType returnType) = 0;
};

class FFI
{
public:
	bool add(const std::string& name, 
		const std::vector<ValueType>& argTypes, 
		ValueType returnType, 
		FFIHandler*);

	enum class RC {
		kOkay,
		kFuncNotFound,
		kIncorrectNumArgs,
		kIncorrectArgType,
		kError
	};
	FFI::RC call(const std::string& name, std::vector<Value>& stack, int nArgs);

private:
	struct FuncDef {
		std::string name;
		std::vector<ValueType> argTypes;
		ValueType returnType;
		FFIHandler* handler;
	};
	std::map<std::string, FuncDef> funcDefs;
};