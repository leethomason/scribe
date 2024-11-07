#pragma once

#include "value.h"

#include <string>
#include <map>

class Environment
{
public:
	Environment() {}

	bool define(const std::string& name, const Value& v);
	bool set(const std::string& name, const Value& v);
	Value get(const std::string& name);
private:
	std::map<std::string, Value> env;
};