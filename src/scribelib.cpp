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

class STDPrint : public FFIHandler
{
public:
	virtual Value call(const std::string& name,
		const std::vector<Value>& args,
		ValueType returnType) override
	{
		(void)returnType;
		(void)name;

		for (size_t i = 0; i < args.size(); i++) {
			fmt::print("{}", args[i].toString());
			if (i < args.size() - 1) {
				fmt::print(", ");
			}
		}
		fmt::print("\n");
		return Value();
	}
};

static STDClock stdClock;
static STDPrint stdPrint;

void AttachStdLib(FFI& ffi, Environment& env)
{
	ffi.add("clock", false, {}, ValueType(PType::tNum), &stdClock, env);
	ffi.add("print2", true, {}, ValueType(PType::tNone), &stdPrint, env);
}

