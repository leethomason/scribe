#pragma once

#include "ast.h"

class Interpreter : public ASTStmtVisitor, public ASTExprVisitor
{
public:
    Value interpret(std::string input);

    virtual void visit(const ASTExprStmtNode&) override;
    virtual void visit(const ASTPrintStmtNode&) override;

	void visit(const ValueASTNode& node, int depth) override;
	void visit(const IdentifierASTNode& node, int depth) override;
	void visit(const KeywordASTNode& node, int depth) override;
	void visit(const BinaryASTNode& node, int depth) override;
	void visit(const UnaryASTNode& node, int depth) override;

	bool hasError() const { return !errorMsg.empty(); }

	std::vector<Value> stack;

private:
	void setErrorMessage(const std::string& msg) { errorMsg = msg; }
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

	std::string errorMsg;
};

