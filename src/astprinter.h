#pragma once

#include "ast.h"

class ASTPrinter : public ASTExprVisitor, public ASTStmtVisitor {
public:
	void visit(const ValueASTNode& node, int depth) override;
	void visit(const IdentifierASTNode& node, int depth) override;
	void visit(const AssignmentASTNode& node, int depth) override;
	void visit(const BinaryASTNode& node, int depth) override;
	void visit(const UnaryASTNode& node, int depth) override;
	void visit(const LogicalASTNode& node, int depth) override;

	void visit(const ASTExprStmtNode&, int depth) override;
	void visit(const ASTPrintStmtNode&, int depth) override;
	void visit(const ASTReturnStmtNode&, int depth) override;
	void visit(const ASTVarDeclStmtNode&, int depth) override;
	void visit(const ASTBlockStmtNode&, int depth) override;
	void visit(const ASTIfStmtNode& node, int depth) override;
	void visit(const ASTWhileStmtNode& node, int depth) override;

	void print(const ASTExprPtr& ast);
};