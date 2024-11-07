#include "instinterp.h"

bool ExprInterpreter::verifyUnderflow(const std::string& ctx, int n)
{
	if (stack.size() < n) {
		setErrorMessage(fmt::format("{}: stack underflow", ctx));
		return false;
	}
	return true;
}

bool ExprInterpreter::verifyTypes(const std::string& ctx, const std::vector<ValueType>& types)
{
	REQUIRE(types.size() > 0);

	if (!verifyUnderflow(ctx, (int)types.size())) return false;
	for (size_t i = 0; i < types.size(); i++) {
		ValueType type = stack[stack.size() - i - 1].type;
		if (type != types[i]) {
			setErrorMessage(fmt::format("{}: expected '{}' at stack -{}", ctx, TypeName(type), i + 1));
			return false;
		}
	}
	return true;
}