#pragma once

#include "ast.h"
#include "environment.h"
#include "func.h"

#include <exception>
#include <stdexcept>

class Interpreter : public ASTStmtVisitor, public ASTExprVisitor
{
public:
	Interpreter();
    Value interpret(const std::string& input, const std::string& contextName);

	// ASTStmtVisitor
    virtual void visit(const ASTExprStmt&, int depth) override;
	virtual void visit(const ASTReturnStmt&, int depth) override;
	virtual void visit(const ASTVarDeclStmt&, int depth) override;
	virtual void visit(const ASTBlockStmt&, int depth) override;
	virtual void visit(const ASTIfStmt& node, int depth) override;
	virtual void visit(const ASTWhileStmt& node, int depth) override;

	// ASTExprVisitor
	void visit(const ASTValueExpr& node, int depth) override;
	void visit(const ASTIdentifierExpr& node, int depth) override;
	void visit(const ASTAssignmentExpr& node, int depth) override;
	void visit(const ASTBinaryExpr& node, int depth) override;
	void visit(const ASTUnaryExpr& node, int depth) override;
	void visit(const ASTLogicalExpr& node, int depth) override;
	void visit(const ASTCallExpr& node, int depth) override;

	std::vector<Value> stack;
	FFI ffi;

private:
	class InterpreterError : public std::runtime_error {
		public:
		InterpreterError(const std::string& msg) : std::runtime_error(msg) {}
	};

	void runtimeError(const std::string& msg);
	void internalError(const std::string& msg);

	bool verifyUnderflow(const std::string& ctx, int n);
	bool verifyTypes(const std::string& ctx, const std::vector<ValueType>& types);	// checks underflow as well
	bool verifyScalarTypes(const std::string& ctx, const std::vector<PType>& types); // checks underflow

	void popStack(int n = 1) {
		REQUIRE(n >= 0);
		REQUIRE(stack.size() >= n);
		stack.resize(stack.size() - n);
	}

	// This comes about because assignment can push to the stack, which
	// leaves something "extra". I wish there was a more elegant solution.
	struct RestoreStack {
		RestoreStack(std::vector<Value>& stack) : stack(stack), size(stack.size()) {}
		~RestoreStack() { stack.resize(size); }
		std::vector<Value>& stack;
		size_t size;
	};

	// Checking that the stack is left as is expected.
	struct CheckStack {
		CheckStack(std::vector<Value>& stack, size_t delta) : stack(stack) {
			expected = stack.size() + delta;
		};
		~CheckStack() { REQUIRE(stack.size() == expected); }
		std::vector<Value>& stack;
		size_t expected = 0;
	};

	Value numberBinaryOp(TokenType op, const Value& lhs, const Value& rhs);
	Value stringBinaryOp(TokenType op, const Value& lhs, const Value& rhs);
	Value boolBinaryOp(TokenType op, const Value& lhs, const Value& rhs);

	// Warning: unchecked!
	Value& getStack(int i) {
		assert(i > 0 && i <= stack.size());
		return stack[stack.size() - i];
	}
	static constexpr int LHS = 2;
	static constexpr int RHS = 1;

	EnvironmentStack env;
	Heap heap;
};

