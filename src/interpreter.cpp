#include "interpreter.h"

#include "token.h"
#include "parser.h"
#include "errorreporting.h"
#include "func.h"
#include "scribelib.h"
#include "astprinter.h"

#define DEBUG_INTERPRETER() 0

Interpreter::Interpreter()
{
	AttachStdLib(ffi, env.globalEnv());
}

Value Interpreter::interpret(const std::string& input, const std::string& ctxName)
{
    Tokenizer tokenizer(input);
#if DEBUG_INTERPRETER()
    tokenizer.debug = true;
#endif	
    Parser parser(tokenizer, ctxName);
    Value rc;

    std::vector<ASTStmtPtr> stmts = parser.parseStmts();

	if (ErrorReporter::hasError()) {
		return Value();
	}

	try {
		for (const auto& stmt : stmts) {
			// Clear the stack at the beginning of the loop so
			// that an expression statement can "return" a result.
			RestoreStack rs(stack);

#if DEBUG_INTERPRETER()
			ASTPrinter printer;
			stmt->accept(printer, 0);
#endif
			stmt->accept(*this, 0);
			if (stack.size() == 1)
				rc = stack[0];
		}
		REQUIRE(stack.size() <= 1);

		// FIXME: metric to clear the heap.
		// Might also make sense at allocation?
	}
	catch (InterpreterError& e) {
		fmt::print("Interpreter run-time error: {}\n", e.what());
	}
	heap.collect();
	if (heap.objects().size() > 0)
		heap.report();

    return rc;
}

void Interpreter::visit(const ASTExprStmt& node, int depth)
{
	CheckStack cs(stack, 1);

    node.expr->accept(*this, depth + 1);
}

void Interpreter::visit(const ASTReturnStmt& node, int depth)
{
	node.expr->accept(*this, depth + 1);
	REQUIRE(stack.size() == 1);
}

void Interpreter::visit(const ASTIfStmt& node, int depth)
{
	node.condition->accept(*this, depth + 1);
	REQUIRE(stack.size() == 1);

	Value cond = stack.back();
	popStack();

	RestoreStack rs(stack);
	if (cond.isTruthy()) {
		node.thenBranch->accept(*this, depth + 1);
	}
	else if (node.elseBranch) {
		node.elseBranch->accept(*this, depth + 1);
	}
}

void Interpreter::visit(const ASTWhileStmt& node, int depth)
{
	while (true) {
		size_t stackSz = stack.size();
		node.condition->accept(*this, depth + 1);
		REQUIRE(stack.size() == stackSz + 1);
		bool truthy = getStack(1).isTruthy();
		popStack();

		if (!truthy)
			break;

		RestoreStack rs(stack);
		node.body->accept(*this, depth + 1);
	}
}

void Interpreter::visit(const ASTBlockStmt& node, int depth)
{
	env.push();
	for (const auto& stmt : node.stmts) {
		stmt->accept(*this, depth + 1);
	}
	env.pop();
}

void Interpreter::visit(const ASTVarDeclStmt& node, int depth)
{
	REQUIRE(stack.empty());

	Value value = Value::Default(node.valueType, heap);

	if (node.expr) {
		RestoreStack rs(stack);

		node.expr->accept(*this, depth + 1);
		REQUIRE(stack.size() == 1);
		REQUIRE(stack[0].type == value.type);	// Should have been established by parser
		value = stack[0];
	}

	if (!env.define(node.name, value)) {
		runtimeError(fmt::format("Env variable {} already defined", node.name));
		return;
	}
}

void Interpreter::runtimeError(const std::string& msg)
{
	ErrorReporter::reportRuntime(msg);
	throw InterpreterError(msg);
}

void Interpreter::internalError(const std::string& msg)
{
	ErrorReporter::reportRuntime(msg);
	throw InterpreterError(msg);
}

bool Interpreter::verifyUnderflow(const std::string& ctx, int n)
{
	if (stack.size() < n) {
		internalError(fmt::format("{}: stack underflow", ctx));
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
			runtimeError(fmt::format("{}: expected '{}' at stack {}", ctx, type.typeName(), i + 1));
			return false;
		}
	}
	return true;
}

bool Interpreter::verifyScalarTypes(const std::string& ctx, const std::vector<PType>& types)
{
	REQUIRE(types.size() > 0);

	if (!verifyUnderflow(ctx, (int)types.size())) return false;
	for (size_t i = 0; i < types.size(); i++) {
		ValueType type = stack[stack.size() - i - 1].type;
		if (type.layout != Layout::tScalar || type.pType != types[i]) {
			runtimeError(fmt::format("{}: expected '{}' at stack {}", ctx, type.typeName(), i + 1));
			return false;
		}
	}
	return true;
}

void Interpreter::visit(const ASTValueExpr& node, int depth)
{
	(void)depth;

	stack.push_back(node.value);
}

void Interpreter::visit(const ASTIdentifierExpr& node, int depth)
{
	(void)depth;

	Value value = env.get(node.name);

	if (value.type == ValueType()) {
		runtimeError(fmt::format("Could not find var: {}", node.name));
		return;
	}
	stack.push_back(value);
}

void Interpreter::visit(const ASTAssignmentExpr& node, int depth)
{
	(void)depth;
	CheckStack cs(stack, 1);	// Assignment leaves a value on the stack. Drives me a little crazy.
	// Really want to make this more elegant.

	node.right->accept(*this, depth + 1);

	Value value = stack.back();

	if (!env.set(node.name, value)) {
		runtimeError(fmt::format("Could not find var: {}", node.name));
		return;
	}
}

Value Interpreter::numberBinaryOp(TokenType op, const Value& lhs, const Value& rhs)
{
	switch (op) {
	case TokenType::PLUS: return Value::Number(lhs.vNumber + rhs.vNumber);
	case TokenType::MINUS: return Value::Number(lhs.vNumber - rhs.vNumber);
	case TokenType::MULT: return Value::Number(lhs.vNumber * rhs.vNumber);
	case TokenType::DIVIDE: return Value::Number(lhs.vNumber / rhs.vNumber);
	case TokenType::GREATER: return Value::Boolean(lhs.vNumber > rhs.vNumber);
	case TokenType::GREATER_EQUAL: return Value::Boolean(lhs.vNumber >= rhs.vNumber);
	case TokenType::LESS: return Value::Boolean(lhs.vNumber < rhs.vNumber);
	case TokenType::LESS_EQUAL: return Value::Boolean(lhs.vNumber <= rhs.vNumber);
	case TokenType::BANG_EQUAL: return Value::Boolean(lhs.vNumber != rhs.vNumber);
	case TokenType::EQUAL_EQUAL: return Value::Boolean(lhs.vNumber == rhs.vNumber);
	default:
		assert(false);
	}
	return Value();
}

Value Interpreter::stringBinaryOp(TokenType op, const Value& lhs, const Value& rhs)
{
	switch (op) {
	case TokenType::PLUS: return Value::String(*lhs.vString + *rhs.vString);
	case TokenType::GREATER: return Value::Boolean(lhs.vString > rhs.vString);
	case TokenType::GREATER_EQUAL: return Value::Boolean(lhs.vString >= rhs.vString);
	case TokenType::LESS: return Value::Boolean(lhs.vString < rhs.vString);
	case TokenType::LESS_EQUAL: return Value::Boolean(lhs.vString <= rhs.vString);
	case TokenType::BANG_EQUAL: return Value::Boolean(lhs.vString != rhs.vString);
	case TokenType::EQUAL_EQUAL: return Value::Boolean(lhs.vString == rhs.vString);
	default:
		assert(false);
	}
	return Value();
}

Value Interpreter::boolBinaryOp(TokenType op, const Value& lhs, const Value& rhs)
{
	switch (op) {
	case TokenType::BANG_EQUAL: return Value::Boolean(lhs.vBoolean != rhs.vBoolean);
	case TokenType::EQUAL_EQUAL: return Value::Boolean(lhs.vBoolean == rhs.vBoolean);
	default:
		assert(false);
	}
	return Value();
}

void Interpreter::visit(const ASTBinaryExpr& node, int depth)
{
	(void)depth;

	node.left->accept(*this, depth + 1);
	node.right->accept(*this, depth + 1);
	if (!verifyUnderflow("BinaryOp", 2)) 
		return;

	const Value& rhs = getStack(RHS);
	const Value& lhs = getStack(LHS);

	if (lhs.type != rhs.type) {
		runtimeError("BinaryOp: type mismatch");
		return;
	}

	Value result;
	assert(lhs.type.layout == Layout::tScalar);	// need to implement reference

	if (lhs.type.pType == PType::tNum) {
		result = numberBinaryOp(node.type, lhs, rhs);
	}
	else if (lhs.type.pType == PType::tStr) {
		result = stringBinaryOp(node.type, lhs, rhs);
	}
	else if (lhs.type.pType == PType::tBool) {
		result = boolBinaryOp(node.type, lhs, rhs);
	}
	else {
		runtimeError("BinaryOp: unhandled type");
	}
	if(result.type == ValueType()) {
		runtimeError("BinaryOp: unhandled op for the type");
		return;
	}

	popStack(2);
	stack.push_back(result);
}

void Interpreter::visit(const ASTUnaryExpr& node, int depth)
{
	(void)depth;
	REQUIRE(node.type == TokenType::MINUS || node.type == TokenType::BANG);

	node.right->accept(*this, depth + 1);
	if (!verifyUnderflow("Unary", 1)) 
		return;

	Value val = getStack(1);
	popStack();

	if (node.type == TokenType::MINUS) {
		if (!verifyScalarTypes("Negative", { PType::tNum })) return;
		stack.push_back(Value::Number(-val.vNumber));
	}
	else if (node.type == TokenType::BANG) {
		bool truthy = val.isTruthy();
		stack.push_back(Value::Boolean(!truthy));
	}
}

void Interpreter::visit(const ASTLogicalExpr& node, int depth)
{
	REQUIRE(node.type == TokenType::LOGIC_AND || node.type == TokenType::LOGIC_OR);
		
	size_t stackSize = stack.size();
	node.left->accept(*this, depth + 1);
	REQUIRE(stack.size() == stackSize + 1);
	bool isTruthy = getStack(1).isTruthy();
	popStack();

	if (node.type == TokenType::LOGIC_OR) {
		if (isTruthy) {
			stack.push_back(Value::Boolean(true));
			return;
		}
	}
	else {
		if (!isTruthy) {
			stack.push_back(Value::Boolean(false));
			return;
		}
	}
	node.right->accept(*this, depth + 1);
	REQUIRE(stack.size() == stackSize + 1);
}

void Interpreter::visit(const ASTCallExpr& node, int depth)
{
	// The "function call" can itself be an expression, which leads to:
	// foo()()
	// for instance. Strange in C++ (although I've certainly see it) but
	// more common in other languages.
	
	// callee
	{
		CheckStack cs(stack, 1);
		node.callee->accept(*this, depth + 1);
	}
	ValueType funcType(PType::tFunc);
	const Value& func = getStack(1);
	if (func.type != funcType) {
		internalError("func has incorrect type");
	}
	std::string funcName = *func.vString;
	popStack();

	// Arguments
	{
		CheckStack cs(stack, node.arguments.size());
		for (size_t i = 0; i < node.arguments.size(); i++) {
			node.arguments[i]->accept(*this, depth + 1);
		}
	}
	
	FFI::RC rc = ffi.call(funcName, stack, (int)node.arguments.size());
	if (rc == FFI::RC::kFuncNotFound) {
		assert(false);
	}
	else if (rc == FFI::RC::kIncorrectNumArgs) {
		runtimeError(fmt::format("Incorrect num args calling '{}'", funcName));
	}
	else if (rc == FFI::RC::kIncorrectArgType) {
		runtimeError(fmt::format("Incorrect arg types calling '{}'", funcName));
	}
	else if (rc == FFI::RC::kError) {
		internalError("internal error from FFI");
	}
}
