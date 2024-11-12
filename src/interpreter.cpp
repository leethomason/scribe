#include "interpreter.h"

#include "token.h"
#include "parser.h"
#include "errorreporting.h"
#include "astprinter.h"
#include "bcgen.h"


Value Interpreter::interpret(const std::string& input, const std::string& ctxName)
{
    Tokenizer tokenizer(input);
    tokenizer.debug = false;
    Parser parser(tokenizer, ctxName);
    Value rc;

    std::vector<ASTStmtPtr> stmts = parser.parseStmts();

    if (ErrorReporter::hasError()) {
        return rc;
    }

    for (const auto& stmt : stmts) {
		assert(stack.size() == 0);
		ASTPrinter printer;
		stmt->accept(printer);

        stmt->accept(*this);
    }
	if (ErrorReporter::hasError()) {
		return rc;
	}

	if (stack.size() == 1) {
		rc = stack[0];
		stack.clear();
	}
	else if (stack.size() > 1) {
		assert(false);
	}
    return rc;
}


void Interpreter::visit(const ASTExprStmtNode&)
{
    assert(false); // need to implement
}

void Interpreter::visit(const ASTPrintStmtNode& node)
{
	node.expr->accept(*this, 0);
    assert(stack.size() == 1);
	std::string s = fmt::format("{}\n", stack[0].toString());
	if (output) *output += s;
	fmt::print(s);
	popStack();
}

void Interpreter::visit(const ASTReturnStmtNode& node)
{
	node.expr->accept(*this, 0);
	assert(stack.size() == 1);
}

void Interpreter::visit(const ASTVarDeclStmtNode& node)
{
	REQUIRE(stack.empty());

	Value value;

	// This assigns default values for types.
	switch (node.valueType) {
	case ValueType::tNumber: value = Value::Number(0.0); break;
	case ValueType::tString: value = Value::String(""); break;
	case ValueType::tBoolean: value = Value::Boolean(false); break;
	default:
		assert(false); // unimplemented? internal error.
	}

	if (node.expr) {
		node.expr->accept(*this, 0);
		REQUIRE(stack.size() == 1);
		REQUIRE(stack[0].type == value.type);	// Should have been established by parser
		value = stack[0];
		stack.clear();
	}

	if (!env.define(node.name, value)) {
		ErrorReporter::reportRuntime(fmt::format("Env variable {} already defined", node.name));
		return;
	}
}

bool Interpreter::verifyUnderflow(const std::string& ctx, int n)
{
	if (stack.size() < n) {
		ErrorReporter::reportRuntime(fmt::format("{}: stack underflow", ctx));
		return false;
	}
	return true;
}

bool Interpreter::verifyTypes(const std::string& ctx, const std::vector<ValueType>& types)
{
	REQUIRE(types.size() > 0);

	if (!verifyUnderflow(ctx, (int)types.size())) return false;
	for (size_t i = 0; i < types.size(); i++) {
		ValueType type = stack[stack.size() - i - 1].type;
		if (type != types[i]) {
			ErrorReporter::reportRuntime(fmt::format("{}: expected '{}' at stack -{}", ctx, TypeName(type), i + 1));
			return false;
		}
	}
	return true;
}

void Interpreter::visit(const ValueASTNode& node, int depth)
{
	(void)depth;
	stack.push_back(node.value);
}

void Interpreter::visit(const IdentifierASTNode& node, int depth)
{
	(void)depth;
	if (ErrorReporter::hasError()) return;

	Value value = env.get(node.name);

	if (value.type == ValueType::tNone) {
		ErrorReporter::reportRuntime(fmt::format("Could not find var: {}", node.name));
		return;
	}
	stack.push_back(value);
}

void Interpreter::visit(const KeywordASTNode& node, int depth)
{
	(void)node;
	(void)depth;
	assert(false);
}

void Interpreter::visit(const BinaryASTNode& node, int depth)
{
	(void)depth;
	if (ErrorReporter::hasError()) return;
	if (!verifyUnderflow("BinaryOp", 2)) return;

	Value rhs = getStack(RHS);
	Value lhs = getStack(LHS);

	if (lhs.type == ValueType::tString) {
		// String concat
		if (!verifyTypes("BinaryOp::String Concat", { ValueType::tString, ValueType::tString })) return;
		popStack(2);

		Value r = Value::String(*lhs.vString + *rhs.vString);
		stack.push_back(r);
	}
	else {
		if (!verifyTypes("BinaryOp", { ValueType::tNumber, ValueType::tNumber })) return;
		popStack(2);

		double right = rhs.vNumber;
		double left = lhs.vNumber;
		double r = 0.0;

		switch (node.type) {
		case TokenType::PLUS: r = left + right; break;
		case TokenType::MINUS: r = left - right; break;
		case TokenType::MULT: r = left * right; break;
		case TokenType::DIVIDE: r = left / right; break;
		default:
			ErrorReporter::reportRuntime("Unexpected Binary Op");
			assert(false);
		}

		stack.push_back(Value::Number(r));
	}
}

void Interpreter::visit(const UnaryASTNode& node, int depth)
{
	(void)depth;
	REQUIRE(node.type == TokenType::MINUS || node.type == TokenType::BANG);
	if (ErrorReporter::hasError()) return;

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
