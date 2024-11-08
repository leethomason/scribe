#include "instinterp.h"

#include <fmt/core.h>

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

void ExprInterpreter::interpret(const ASTExprNode& node)
{
	node.accept(*this, 0);
}

void ExprInterpreter::visit(const ValueASTNode& node, int depth)
{
	(void)depth;
	stack.push_back(node.value);
}

void ExprInterpreter::visit(const IdentifierASTNode& node, int depth)
{
	(void)node;
	(void)depth;
	assert(false);
}

void ExprInterpreter::visit(const KeywordASTNode& node, int depth)
{
	(void)node;
	(void)depth;
	assert(false);
}

void ExprInterpreter::visit(const BinaryASTNode& node, int depth)
{
	(void)depth;
	if (hasError()) return;
	if (!verifyUnderflow("BinaryOp", 2)) return;

	Value rightV = getStack(RHS);
	Value leftV = getStack(LHS);
	popStack(2);

	if (getStack(LHS).type == ValueType::tString) {
		// String concat
		if (!verifyTypes("BinaryOp::String Concat", { ValueType::tString, ValueType::tString })) return;

		Value r = Value::String(*rightV.vString + *leftV.vString);
		stack.push_back(r);
	}
	else {
		if (!verifyTypes("BinaryOp", { ValueType::tNumber, ValueType::tNumber })) return;

		double right = rightV.vNumber;
		double left = leftV.vNumber;
		double r = 0.0;

		switch (node.type) {
		case TokenType::PLUS: r = left + right; break;
		case TokenType::MINUS: r = left - right; break;
		case TokenType::MULT: r = left * right; break;
		case TokenType::DIVIDE: r = left / right; break;
		default:
			setErrorMessage("Unexpected Binary Op");
			assert(false);
		}

		stack.push_back(Value::Number(r));
	}
}

void ExprInterpreter::visit(const UnaryASTNode& node, int depth)
{
	(void)depth;
	REQUIRE(node.type == TokenType::MINUS || node.type == TokenType::BANG);
	if (hasError()) return;

	if (!verifyUnderflow("Unary", 1)) return;

	Value val = getStack(1);
	popStack();

	if (node.type == TokenType::MINUS) {
		if (!verifyTypes("Negative", { ValueType::tNumber })) return;
		stack.push_back(Value::Number(-val.vNumber));
	}
	else if (node.type == TokenType::BANG) {
		bool truthy = val.isTruthy();
		stack.push_back(Value::Boolean(!truthy));
	}
}

void StmtInterpreter::interpret(const ASTStmtNode& node)
{
	node.accept(*this, 0);
}

void StmtInterpreter::visit(const ASTExprStmtNode& node, int depth)
{
	(void)depth;
	ExprInterpreter exprInterp;
	exprInterp.interpret(*node.expr);
	assert(exprInterp.stack.size() == 1);
	stack.push_back(exprInterp.stack[0]);
}

void StmtInterpreter::visit(const ASTPrintStmtNode& node, int depth)
{
	(void)depth;
	ExprInterpreter exprInterp;
	exprInterp.interpret(*node.expr);
	assert(exprInterp.stack.size() == 1);
	fmt::print("print: {}\n", exprInterp.stack[0].toString());
}



