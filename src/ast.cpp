#include "ast.h"
#include "error.h"

void ValueASTNode::evaluate(std::vector<Instruction>& bc, ConstPool& pool)
{
	uint32_t slot = pool.add(value);
	bc.push_back(PackOpCode(OpCode::PUSH, slot));
}

void IdentifierASTNode::evaluate(std::vector<Instruction>& bc, ConstPool& pool)
{
	uint32_t slot = pool.add(Value::String(name));
	bc.push_back(PackOpCode(OpCode::PUSH, slot));
	bc.push_back(PackOpCode(OpCode::LOAD));
}

void BinaryASTNode::evaluate(std::vector<Instruction>& bc, ConstPool& pool)
{
	REQUIRE(left);
	REQUIRE(right);

	left->evaluate(bc, pool);
	right->evaluate(bc, pool);

	switch (type) {
	case TokenType::PLUS: bc.push_back(PackOpCode(OpCode::ADD)); break;
	case TokenType::MINUS: bc.push_back(PackOpCode(OpCode::SUB)); break;
	case TokenType::MULT: bc.push_back(PackOpCode(OpCode::MUL)); break;
	case TokenType::DIVIDE: bc.push_back(PackOpCode(OpCode::DIV)); break;
	default: REQUIRE(false);
	}
}

