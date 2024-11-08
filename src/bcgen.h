#pragma once

#include "ast.h"

class BCExprGenerator : public ASTExprVisitor {
public:
	BCExprGenerator(std::vector<Instruction>& bc, ConstPool& pool) : bc(bc), pool(pool) {}
	void generate(const ASTExprNode& node);

	void visit(const ValueASTNode& node, int depth) override;
	void visit(const IdentifierASTNode& node, int depth) override;
	void visit(const KeywordASTNode& node, int depth) override;
	void visit(const BinaryASTNode& node, int depth) override;
	void visit(const UnaryASTNode& node, int depth) override;

private:
	std::vector<Instruction>& bc;
	ConstPool& pool;
};

class BCStmtGenerator : public ASTStmtVisitor {
public:
	BCStmtGenerator(std::vector<Instruction>& bc, ConstPool& pool) : bc(bc), pool(pool) {}
	void generate(const ASTStmtNode& node);

	void visit(const ASTExprStmtNode& node) override;
	void visit(const ASTPrintStmtNode& node) override;

private:
	std::vector<Instruction>& bc;
	ConstPool& pool;
};
