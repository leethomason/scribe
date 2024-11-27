#include "astprinter.h"

#include <fmt/core.h>

void ASTPrinter::visit(const ASTExprStmtNode& node, int depth)
{
	fmt::print("STMT expr\n");
	node.expr->accept(*this, depth + 1);
}

void ASTPrinter::visit(const ASTPrintStmtNode& node, int depth)
{
	fmt::print("STMT print\n");
	node.expr->accept(*this, depth + 1);
}

void ASTPrinter::visit(const ASTReturnStmtNode& node, int depth)
{
	fmt::print("STMT return\n");
	node.expr->accept(*this, depth + 1);
}

void ASTPrinter::visit(const ASTBlockStmtNode& node, int depth)
{
	fmt::print("STMT block {{\n");
	for (const auto& stmt : node.stmts)
		stmt->accept(*this, depth + 1);
	fmt::print("}}\n");
}

void ASTPrinter::visit(const ASTIfStmtNode& node, int depth)
{
	fmt::print("STMT if\n");
	node.condition->accept(*this, depth + 1);
	node.thenBranch->accept(*this, depth + 1);
	if (node.elseBranch) {
		fmt::print("     else\n");
		node.elseBranch->accept(*this, depth + 1);
	}
}

void ASTPrinter::visit(const ASTWhileStmtNode& node, int depth)
{
	fmt::print("STMT while\n");
	fmt::print("  Condition\n");
	node.condition->accept(*this, depth + 1);
	fmt::print("  Body\n");
	node.body->accept(*this, depth + 1);
}

void ASTPrinter::visit(const ASTVarDeclStmtNode& node, int depth)
{
	fmt::print("STMT var decl: {}: {}\n", node.name, node.valueType.typeName());
	if (node.expr)
		node.expr->accept(*this, depth + 1);
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
