#pragma once

#include "ast.h"

class ASTPrinter : public ASTVisitor {
public:
	void visit(const ValueASTNode& node, int depth) override;
	void visit(const IdentifierASTNode& node, int depth) override;
	void visit(const BinaryASTNode& node, int depth) override;

	void print(const ASTPtr& ast);
};