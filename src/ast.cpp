#include "ast.h"
#include "error.h"

void ValueNode::evaluate(std::vector<Instruction>& bc, ConstPool& pool)
{
	uint32_t slot = pool.add(value);
	bc.push_back(PackOpCode(OpCode::PUSH, slot));
}

void DeclareVariableNode::evaluate(std::vector<Instruction>& bc, ConstPool& pool)
{
	uint32_t slot = pool.add(Value::String(name));
	bc.push_back(PackOpCode(OpCode::PUSH, slot));	// push the name
	expr->evaluate(bc, pool);						// push the value
	// FIXME: local or global?
	bc.push_back(PackOpCode(OpCode::DEFINE_LOCAL));
}

void AssignmentNode::evaluate(std::vector<Instruction>& bc, ConstPool& pool)
{
	uint32_t slot = pool.add(Value::String(varName));
	expr->evaluate(bc, pool);
	bc.push_back(PackOpCode(OpCode::STORE, slot));
}

void BinaryOpNode::evaluate(std::vector<Instruction>& bc, ConstPool& pool)
{
	left->evaluate(bc, pool);
	right->evaluate(bc, pool);

	switch (type) {
	case TokenType::plus: bc.push_back(PackOpCode(OpCode::ADD)); break;
	case TokenType::minus: bc.push_back(PackOpCode(OpCode::SUB)); break;
	case TokenType::multiply: bc.push_back(PackOpCode(OpCode::MUL)); break;
	case TokenType::divide: bc.push_back(PackOpCode(OpCode::DIV)); break;
	default: REQUIRE(false);
	}
}

