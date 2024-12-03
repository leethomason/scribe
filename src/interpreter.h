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
    virtual void visit(const ASTExprStmtNode&, int depth) override;
    virtual void visit(const ASTPrintStmtNode&, int depth) override;
	virtual void visit(const ASTReturnStmtNode&, int depth) override;
	virtual void visit(const ASTVarDeclStmtNode&, int depth) override;
	virtual void visit(const ASTBlockStmtNode&, int depth) override;
	virtual void visit(const ASTIfStmtNode& node, int depth) override;
	virtual void visit(const ASTWhileStmtNode& node, int depth) override;

	// ASTExprVisitor
	void visit(const ValueASTNode& node, int depth) override;
	void visit(const IdentifierASTNode& node, int depth) override;
	void visit(const AssignmentASTNode& node, int depth) override;
	void visit(const BinaryASTNode& node, int depth) override;
	void visit(const UnaryASTNode& node, int depth) override;
	void visit(const LogicalASTNode& node, int depth) override;
	void visit(const CallASTNode& node, int depth) override;

	void setOutput(std::string& out) { output = &out; }

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

	std::string* output = nullptr;

	EnvironmentStack env;
	Heap heap;
};

