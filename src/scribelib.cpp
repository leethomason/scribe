#include "scribelib.h"
#include "func.h"

#include <chrono>

class STDClock : public FFIHandler
{
public:
	virtual Value call(const std::string& name,
		const std::vector<Value>& args,
		ValueType returnType) override
	{
		(void)returnType;
		(void)args;
		REQUIRE(name == "clock");

		// FIXME some performance conter that isn't terrible
		auto now = std::chrono::high_resolution_clock::now();
		auto ms = std::chrono::time_point_cast<std::chrono::microseconds>(now).time_since_epoch().count();
		uint64_t msU64 = static_cast<uint64_t>(ms);

		return Value::Number(msU64 / (1000.0 * 1000.0));
	}
};

class STDFormatBase : public FFIHandler
{
public:
	std::string format(const std::vector<Value>& args) 
	{
		std::string rc;

		for (size_t i = 0; i < args.size(); i++) {
			rc += fmt::format("{}", args[i].toString());
			if (i < args.size() - 1) {
				rc += ", ";
			}
		}
		return rc;
	}
};

class STDPrint : public STDFormatBase
{
public:
	virtual Value call(const std::string& name,
		const std::vector<Value>& args,
		ValueType returnType) override
	{
		(void)returnType;
		(void)name;

		std::string str = format(args);
		fmt::print("{}\n", str);

		return Value();
	}
};

class STDFormat : public STDFormatBase
{
public:
	virtual Value call(const std::string& name,
		const std::vector<Value>& args,
		ValueType returnType) override
	{
		(void)returnType;
		(void)name;

		std::string str = format(args);
		return Value::String(str);
	}
};

static STDClock stdClock;
static STDPrint stdPrint;
static STDFormat stdFormat;

void AttachStdLib(FFI& ffi, Environment& env)
{
	ffi.add("clock", false, {}, ValueType(PType::tNum), &stdClock, env);
	ffi.add("print", true, {}, ValueType(PType::tNone), &stdPrint, env);
	ffi.add("format", true, {}, ValueType(PType::tStr), &stdFormat, env);
}

