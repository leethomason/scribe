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

static STDClock stdClock;

void AttachStdLib(FFI& ffi, Environment& env)
{
	ffi.add("clock", {}, ValueType(PType::tNum), &stdClock, env);
}

