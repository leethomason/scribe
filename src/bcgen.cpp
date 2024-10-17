#include "bcgen.h"

void BCGenerator::visit(const ValueASTNode& node, int depth)
{
	(void)depth;
	uint32_t slot = pool.add(node.value);
	bc.push_back(PackOpCode(OpCode::PUSH, slot));
}

void BCGenerator::visit(const IdentifierASTNode& node, int depth)
{
	(void)depth;
	uint32_t slot = pool.add(Value::String(node.name));
	bc.push_back(PackOpCode(OpCode::PUSH, slot));
	bc.push_back(PackOpCode(OpCode::LOAD));
}

void BCGenerator::visit(const KeywordASTNode& node, int depth)
{
	(void)depth;
	switch (node.token) {
		case TokenType::PRINT: bc.push_back(PackOpCode(OpCode::PRINT)); break;
	default: 
		REQUIRE(false);
	}
}

void BCGenerator::visit(const BinaryASTNode& node, int depth)
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

void BCGenerator::visit(const UnaryASTNode& node, int depth)
{
	(void)depth;

	// These are alreaded 'visited' 
	REQUIRE(node.right);

	// So we just need to ad the operation.
//	switch (node.type) {
//	case TokenType::MINUS: bc.push_back(PackOpCode(OpCode::NEG)); break;
//	case TokenType::BANG: bc.push_back(PackOpCode(OpCode::NOT)); break;
//	default: REQUIRE(false);
//	}
	assert(false);
}

void BCGenerator::generate(const ASTNode& node)
{
	node.accept(*this, 0);
}
