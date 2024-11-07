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

