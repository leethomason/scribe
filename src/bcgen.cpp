#include "bcgen.h"

void BCExprGenerator::visit(const ASTValueExpr& node, int depth)
{
	(void)depth;
	uint32_t slot = pool.add(node.value);
	bc.push_back(PackOpCode(OpCode::PUSH, slot));
}

void BCExprGenerator::visit(const ASTIdentifierExpr& node, int depth)
{
	(void)depth;
	uint32_t slot = pool.add(Value::String(node.name));
	bc.push_back(PackOpCode(OpCode::PUSH, slot));
	bc.push_back(PackOpCode(OpCode::LOAD));
}

void BCExprGenerator::visit(const ASTBinaryExpr& node, int depth)
{
	(void)depth;

	// These are alreaded 'visited' 
	REQUIRE(node.left);
	REQUIRE(node.right);

	// So we just need to ad the operation.
	switch (node.type) {
	case TokenType::PLUS: bc.push_back(PackOpCode(OpCode::ADD)); break;
	case TokenType::MINUS: bc.push_back(PackOpCode(OpCode::SUB)); break;
	case TokenType::MULT: bc.push_back(PackOpCode(OpCode::MUL)); break;
	case TokenType::DIVIDE: bc.push_back(PackOpCode(OpCode::DIV)); break;
	default: REQUIRE(false);
	}
}

void BCExprGenerator::visit(const ASTUnaryExpr& node, int depth)
{
	(void)depth;

	// These are alreaded 'visited' 
	REQUIRE(node.right);

	// So we just need to ad the operation.
	switch (node.type) {
	case TokenType::MINUS: bc.push_back(PackOpCode(OpCode::NEGATE)); break;
	case TokenType::BANG: bc.push_back(PackOpCode(OpCode::NOT)); break;
	default: REQUIRE(false);
	}
}

void BCExprGenerator::generate(const ASTExprNode& node)
{
	node.accept(*this, 0);
}
