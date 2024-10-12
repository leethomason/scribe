#include "ast.h"
#include "error.h"

void ValueASTNode::evaluate(std::vector<Instruction>& bc, ConstPool& pool)
{
	REQUIRE(!left);
	REQUIRE(!right);

	uint32_t slot = pool.add(value);
	bc.push_back(PackOpCode(OpCode::PUSH, slot));
}

void ValueASTNode::dump(int depth) const
{
	fmt::print("{: >{}}", "", depth * 2);
	fmt::print("Value: {}\n", value.toString());
}


void IdentifierASTNode::evaluate(std::vector<Instruction>& bc, ConstPool& pool)
{
	REQUIRE(!left);
	REQUIRE(!right);

	uint32_t slot = pool.add(Value::String(name));
	bc.push_back(PackOpCode(OpCode::PUSH, slot));
	bc.push_back(PackOpCode(OpCode::LOAD));
}

void IdentifierASTNode::dump(int depth) const
{
	fmt::print("{: >{}}", "", depth * 2);
	fmt::print("Identifier: {}\n", name);
}

void BinaryASTNode::evaluate(std::vector<Instruction>& bc, ConstPool& pool)
{
	REQUIRE(left);
	REQUIRE(right);

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

void BinaryASTNode::dump(int depth) const
{
	left->dump(depth + 1);
	right->dump(depth + 1);

	std::string opName;
	switch (type) {
	case TokenType::plus: opName = "Add"; break;
	case TokenType::minus: opName = "Sub"; break;
	case TokenType::multiply: opName = "Mul"; break;
	case TokenType::divide: opName = "Div"; break;
	default: REQUIRE(false);
	}

	fmt::print("{: >{}}", "", depth * 2);
	fmt::print("Binary: {}\n", opName);
}
