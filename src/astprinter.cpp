#include "astprinter.h"

#include <fmt/core.h>

void ASTPrinter::visit(const ASTExprStmtNode&)
{
	assert(false);
}

void ASTPrinter::visit(const ASTPrintStmtNode& node)
{
	fmt::print("STMT print\n");
	node.expr->accept(*this, 1);
}

void ASTPrinter::visit(const ASTReturnStmtNode& node)
{
	fmt::print("STMT return\n");
	node.expr->accept(*this, 1);
}

void ASTPrinter::visit(const ASTVarDeclStmtNode& node)
{
	fmt::print("STMT var decl: {}: {}\n", node.name, TypeName(node.valueType));
	if (node.expr)
		node.expr->accept(*this, 1);
}

void ASTPrinter::visit(const ValueASTNode& node, int depth)
{
	fmt::print("{: >{}}", "", depth * 2);
	fmt::print("Value: {}\n", node.value.toString());
}

void ASTPrinter::visit(const IdentifierASTNode& node, int depth)
{
	fmt::print("{: >{}}", "", depth * 2);
	fmt::print("Identifier: {}\n", node.name);
}

void ASTPrinter::visit(const KeywordASTNode& node, int depth)
{
	fmt::print("{: >{}}", "", depth * 2);
	fmt::print("Keyword: {}\n", Token::toString(node.token));
}

void ASTPrinter::visit(const BinaryASTNode& node, int depth)
{
	std::string opName;
	switch (node.type) {
	case TokenType::PLUS: opName = "Add"; break;
	case TokenType::MINUS: opName = "Sub"; break;
	case TokenType::MULT: opName = "Mul"; break;
	case TokenType::DIVIDE: opName = "Div"; break;
	default: REQUIRE(false);
	}

	fmt::print("{: >{}}", "", depth * 2);
	fmt::print("Binary: {}\n", opName);
}

void ASTPrinter::visit(const UnaryASTNode& node, int depth)
{
	std::string opName;
	switch (node.type) {
	case TokenType::MINUS: opName = "Neg"; break;
	case TokenType::BANG: opName = "Bang"; break;
	default: REQUIRE(false);
	}

	fmt::print("{: >{}}", "", depth * 2);
	fmt::print("Unary: {}\n", opName);
}

void ASTPrinter::print(const ASTExprPtr& ast)
{
	ast->accept(*this, 0);
}
