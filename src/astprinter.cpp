#include "astprinter.h"

#include <fmt/core.h>

void ASTPrinter::visit(const ASTExprStmt& node, int depth)
{
	fmt::print("STMT expr\n");
	node.expr->accept(*this, depth + 1);
}

void ASTPrinter::visit(const ASTReturnStmt& node, int depth)
{
	fmt::print("STMT return\n");
	node.expr->accept(*this, depth + 1);
}

void ASTPrinter::visit(const ASTBlockStmt& node, int depth)
{
	fmt::print("STMT block {{\n");
	for (const auto& stmt : node.stmts)
		stmt->accept(*this, depth + 1);
	fmt::print("}}\n");
}

void ASTPrinter::visit(const ASTIfStmt& node, int depth)
{
	fmt::print("STMT if\n");
	node.condition->accept(*this, depth + 1);
	node.thenBranch->accept(*this, depth + 1);
	if (node.elseBranch) {
		fmt::print("     else\n");
		node.elseBranch->accept(*this, depth + 1);
	}
}

void ASTPrinter::visit(const ASTWhileStmt& node, int depth)
{
	fmt::print("STMT while\n");
	fmt::print("  Condition\n");
	node.condition->accept(*this, depth + 1);
	fmt::print("  Body\n");
	node.body->accept(*this, depth + 1);
}

void ASTPrinter::visit(const ASTVarDeclStmt& node, int depth)
{
	fmt::print("STMT var decl: {}: {}\n", node.name, node.valueType.typeName());
	if (node.expr)
		node.expr->accept(*this, depth + 1);
}

void ASTPrinter::visit(const ASTValueExpr& node, int depth)
{
	fmt::print("{: >{}}", "", depth * 2);
	fmt::print("Value: {}\n", node.value.toString());
}

void ASTPrinter::visit(const ASTIdentifierExpr& node, int depth)
{
	fmt::print("{: >{}}", "", depth * 2);
	fmt::print("Identifier: {}\n", node.name);
}

void ASTPrinter::visit(const ASTAssignmentExpr& node, int depth)
{
	fmt::print("{: >{}}", "", depth * 2);
	fmt::print("Assignment: {}\n", node.name);

	node.right->accept(*this, depth + 1);
}

void ASTPrinter::visit(const ASTBinaryExpr& node, int depth)
{
	std::string opName = Token::toString(node.type);
	fmt::print("{: >{}}", "", depth * 2);
	fmt::print("Binary: {}\n", opName);

	node.left->accept(*this, depth + 1);
	node.right->accept(*this, depth + 1);
}

void ASTPrinter::visit(const ASTUnaryExpr& node, int depth)
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

void ASTPrinter::visit(const ASTLogicalExpr& node, int depth)
{
	std::string opName = Token::toString(node.type);

	fmt::print("{: >{}}", "", depth * 2);
	fmt::print("Logical: {}\n", opName);

	node.left->accept(*this, depth + 1);
	node.right->accept(*this, depth + 1);
}

void ASTPrinter::visit(const ASTCallExpr& node, int depth)
{
	fmt::print("{: >{}}", "", depth * 2);
	fmt::print("Call: \n");
	node.callee->accept(*this, depth + 1);
}

void ASTPrinter::print(const ASTExprPtr& ast)
{
	ast->accept(*this, 0);
}

