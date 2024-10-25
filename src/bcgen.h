#pragma once

#include "ast.h"

class BCGenerator : public ASTExprVisitor {
public:
	BCGenerator(std::vector<Instruction>& bc, ConstPool& pool) : bc(bc), pool(pool) {}
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