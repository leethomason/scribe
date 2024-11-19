#pragma once

#include "ast.h"
#include "environment.h"

class Interpreter : public ASTStmtVisitor, public ASTExprVisitor
{
public:
    Value interpret(const std::string& input, const std::string& contextName);

	// ASTStmtVisitor
    virtual void visit(const ASTExprStmtNode&) override;
    virtual void visit(const ASTPrintStmtNode&) override;
	virtual void visit(const ASTReturnStmtNode&) override;
	virtual void visit(const ASTVarDeclStmtNode&) override;
	virtual void visit(const ASTBlockStmtNode&) override;
	virtual void visit(const ASTIfStmtNode& node) override;

	// ASTExprVisitor
	void visit(const ValueASTNode& node, int depth) override;
	void visit(const IdentifierASTNode& node, int depth) override;
	void visit(const AssignmentASTNode& node, int depth) override;
	void visit(const BinaryASTNode& node, int depth) override;
	void visit(const UnaryASTNode& node, int depth) override;

	void setOutput(std::string& out) { output = &out; }

	std::vector<Value> stack;

private:
	void runtimeError(const std::string& msg);
	void internalError(const std::string& msg);

	bool verifyUnderflow(const std::string& ctx, int n);
	bool verifyTypes(const std::string& ctx, const std::vector<ValueType>& types);	// checks underflow as well

	void popStack(int n = 1) {
		REQUIRE(n >= 0);
		REQUIRE(stack.size() >= n);
		stack.resize(stack.size() - n);
	}

	// Warning: unchecked!
	Value& getStack(int i) {
		assert(i > 0 && i <= stack.size());
		return stack[stack.size() - i];
	}
	static constexpr int LHS = 2;
	static constexpr int RHS = 1;

	bool interpreterOkay = true;
	std::string* output = nullptr;

	EnvironmentStack env;
};

