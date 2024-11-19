#pragma once

#include "ast.h"

class ASTPrinter : public ASTExprVisitor, public ASTStmtVisitor {
public:
	void visit(const ValueASTNode& node, int depth) override;
	void visit(const IdentifierASTNode& node, int depth) override;
	void visit(const AssignmentASTNode& node, int depth) override;
	void visit(const BinaryASTNode& node, int depth) override;
	void visit(const UnaryASTNode& node, int depth) override;

	void visit(const ASTExprStmtNode&) override;
	void visit(const ASTPrintStmtNode&) override;
	void visit(const ASTReturnStmtNode&) override;
	void visit(const ASTVarDeclStmtNode&) override;
	void visit(const ASTBlockStmtNode&) override;

	void print(const ASTExprPtr& ast);
};