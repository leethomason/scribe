#include "astprinter.h"

#include <fmt/core.h>

void ASTPrinter::visit(const ASTExprStmtNode& node)
{
	fmt::print("STMT expr\n");
	node.expr->accept(*this, 1);
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

void ASTPrinter::visit(const ASTBlockStmtNode& node)
{
	fmt::print("STMT block {{\n");
	for (const auto& stmt : node.stmts)
		stmt->accept(*this);
	fmt::print("}}\n");
}

void ASTPrinter::visit(const ASTIfStmtNode& node)
{
	fmt::print("STMT if\n");
	node.condition->accept(*this, 1);
	node.thenBranch->accept(*this);
	if (node.elseBranch) {
		fmt::print("     else\n");
		node.elseBranch->accept(*this);
	}
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

void ASTPrinter::visit(const AssignmentASTNode& node, int depth)
{
	fmt::print("{: >{}}", "", depth * 2);
	fmt::print("Assignment: {}\n", node.name);

	node.right->accept(*this, depth + 1);
}

void ASTPrinter::visit(const BinaryASTNode& node, int depth)
{
	std::string opName = Token::toString(node.type);
	fmt::print("{: >{}}", "", depth * 2);
	fmt::print("Binary: {}\n", opName);

	node.left->accept(*this, depth + 1);
	node.right->accept(*this, depth + 1);
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

	node.right->accept(*this, depth + 1);
}

void ASTPrinter::visit(const LogicalASTNode& node, int depth)
{
	std::string opName = Token::toString(node.type);

	fmt::print("{: >{}}", "", depth * 2);
	fmt::print("Logical: {}\n", opName);

	node.left->accept(*this, depth + 1);
	node.right->accept(*this, depth + 1);
}

void ASTPrinter::print(const ASTExprPtr& ast)
{
	ast->accept(*this, 0);
}
