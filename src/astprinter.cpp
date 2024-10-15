#include "astprinter.h"

#include <fmt/core.h>

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

void ASTPrinter::print(const ASTPtr& ast)
{
	ast->accept(*this, 0);
}
