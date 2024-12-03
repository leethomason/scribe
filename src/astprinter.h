#pragma once

#include "ast.h"

class ASTPrinter : public ASTExprVisitor, public ASTStmtVisitor {
public:
	void visit(const ASTValueExpr& node, int depth) override;
	void visit(const ASTIdentifierExpr& node, int depth) override;
	void visit(const ASTAssignmentExpr& node, int depth) override;
	void visit(const ASTBinaryExpr& node, int depth) override;
	void visit(const ASTUnaryExpr& node, int depth) override;
	void visit(const ASTLogicalExpr& node, int depth) override;
	void visit(const ASTCallExpr& node, int depth) override;

	void visit(const ASTExprStmt&, int depth) override;
	void visit(const ASTReturnStmt&, int depth) override;
	void visit(const ASTVarDeclStmt&, int depth) override;
	void visit(const ASTBlockStmt&, int depth) override;
	void visit(const ASTIfStmt& node, int depth) override;
	void visit(const ASTWhileStmt& node, int depth) override;

	void print(const ASTExprPtr& ast);
};