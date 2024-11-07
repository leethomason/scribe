#pragma once

#include "ast.h"

class ExprInterpreter : public ASTExprVisitor {
public:
	ExprInterpreter() {}
	void interpret(const ASTExprNode& node);

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

	std::string errorMsg;
};

class StmtInterpreter : public ASTStmtVisitor {
public:
	StmtInterpreter() {}
	void interpret(const ASTStmtNode& node);

	void visit(const ASTExprStmtNode& node, int depth) override;
	void visit(const ASTPrintStmtNode& node, int depth) override;

private:
};
