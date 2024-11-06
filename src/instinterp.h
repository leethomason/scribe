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

private:
};

class StmtInterpreter : public ASTStmtVisitor {
public:
	StmtInterpreter() {}
	void interpret(const ASTStmtNode& node);

	void visit(const ASTExprStmtNode& node, int depth) override;
	void visit(const ASTPrintStmtNode& node, int depth) override;

private:
};
