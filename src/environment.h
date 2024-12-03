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

class EnvironmentStack
{
public:
	EnvironmentStack() { push(); }
	
	void push();
	void pop();
	
	bool define(const std::string& name, const Value& v);
	bool set(const std::string& name, const Value& v);
	Value get(const std::string& name);

	Environment& globalEnv() { return stack[0]; }

private:
	std::vector<Environment> stack;
};