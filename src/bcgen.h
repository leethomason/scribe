#pragma once

#include "ast.h"

class BCExprGenerator : public ASTExprVisitor {
public:
	BCExprGenerator(std::vector<Instruction>& bc, ConstPool& pool) : bc(bc), pool(pool) {}
	void generate(const ASTExprNode& node);

	void visit(const ASTValueExpr& node, int depth) override;
	void visit(const ASTIdentifierExpr& node, int depth) override;
	void visit(const ASTBinaryExpr& node, int depth) override;
	void visit(const ASTUnaryExpr& node, int depth) override;

private:
	std::vector<Instruction>& bc;
	ConstPool& pool;
};

class BCStmtGenerator : public ASTStmtVisitor {
public:
	BCStmtGenerator(std::vector<Instruction>& bc, ConstPool& pool) : bc(bc), pool(pool) {}
	void generate(const ASTStmtNode& node);

	void visit(const ASTExprStmt& node, int) override;

private:
	std::vector<Instruction>& bc;
	ConstPool& pool;
};
