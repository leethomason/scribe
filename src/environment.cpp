#include "environment.h"

bool Environment::define(const std::string& name, const Value& v)
{
	auto it = env.find(name);
	if (it != env.end()) return false;
	env[name] = v;
	return true;
}

bool Environment::set(const std::string& name, const Value& v)
{
	auto it = env.find(name);
	if (it == env.end()) return false;
	it->second = v;
	return true;
}

Value Environment::get(const std::string& name)
{
	Value v;
	auto it = env.find(name);
	if (it != env.end())
		v = it->second;
	return v;
}

void EnvironmentStack::push()
{
	stack.push_back(Environment());
}

void EnvironmentStack::pop()
{
	stack.pop_back();
}

bool EnvironmentStack::define(const std::string& name, const Value& v)
{
	REQUIRE(!stack.empty());
	return stack.back().define(name, v);
}

bool EnvironmentStack::set(const std::string& name, const Value& v)
{
	for (auto it = stack.rbegin(); it != stack.rend(); ++it) {
		if (it->set(name, v)) 
			return true;
	}
	return false;
}

Value EnvironmentStack::get(const std::string& name)
{
	Value v;
	for (auto it = stack.rbegin(); it != stack.rend(); ++it) {
		v = it->get(name);
		if (v.type != ValueType::tNone)
			break;
	}
	return v;
}
