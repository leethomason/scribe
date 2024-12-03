#include "func.h"
#include "errorreporting.h"

#include <fmt/core.h>

bool FFI::add(const std::string& name, 
	const std::vector<ValueType>& argTypes, 
	ValueType returnType, 
	FFIHandler* handler)
{
	REQUIRE(!name.empty());
	REQUIRE(handler);

	if (funcDefs.find(name) != funcDefs.end()) {
		ErrorReporter::report("FFI", 0, fmt::format("FFI func {} multiply defined", name));
		assert(false);
		return false;
	}

	FuncDef def = {
		name,
		argTypes,
		returnType,
		handler
	};
	funcDefs[name] = def;
	return true;
}

FFI::RC FFI::call(const std::string& name, std::vector<Value>& stack, int nArgs)
{
	auto it = funcDefs.find(name);
	if (it == funcDefs.end()) {
		return RC::kFuncNotFound;;
	}
	const FuncDef& funcDef = it->second;

	if (stack.size() < nArgs || nArgs != funcDef.argTypes.size()) {
		return RC::kIncorrectNumArgs;
	}

	std::vector<Value> args(nArgs);
	for (int i = 0; i < nArgs; ++i) {
		const Value& value = stack[stack.size() - 1 - i];
		if (value.type != funcDef.argTypes[i]) {
			return RC::kIncorrectArgType;
		}
		args[i] = value;
	}
	stack.resize(stack.size() - nArgs);
	Value rc = funcDef.handler->call(name, args, funcDef.returnType);
	stack.push_back(rc);
	return RC::kOkay;
}


