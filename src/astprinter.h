#pragma once

#include "ast.h"

class ASTPrinter : public ASTExprVisitor {
public:
	void visit(const ValueASTNode& node, int depth) override;
	void visit(const IdentifierASTNode& node, int depth) override;
	void visit(const KeywordASTNode& node, int depth) override;
	void visit(const BinaryASTNode& node, int depth) override;
	void visit(const UnaryASTNode& node, int depth) override;

	void print(const ASTExprPtr& ast);
};