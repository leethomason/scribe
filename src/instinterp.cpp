#include "instinterp.h"

void ExprInterpreter::interpret(const ASTExprNode& node)
{
	node.accept(*this, 0);
}

void ExprInterpreter::visit(const ValueASTNode& node, int depth)
{
	(void)depth;
	stack.push_back(node.value);
}

void ExprInterpreter::visit(const IdentifierASTNode& node, int depth)
{
	(void)node;
	(void)depth;
	assert(false);
}

void ExprInterpreter::visit(const KeywordASTNode& node, int depth)
{
	(void)node;
	(void)depth;
	assert(false);
}

void ExprInterpreter::visit(const BinaryASTNode& node, int depth)
{
	Value rightV = stack.back();
	stack.pop_back();
	Value leftV = stack.back();
	stack.pop_back();

	double right = rightV.vNumber;
	double left = leftV.vNumber;
	double r = 0.0;

	switch (node.type) {
	case TokenType::PLUS: r = left + right; break;
	case TokenType::MINUS: r = left - right; break;
	case TokenType::MULT: r = left * right; break;
	case TokenType::DIVIDE: r = left / right; break;
	default: assert(false);
	}

	stack.push_back(Value::Number(r));
}

void ExprInterpreter::visit(const UnaryASTNode& node, int depth)
{
	Value rightV = stack.back();
	stack.pop_back();

	double right = rightV.vNumber;
	double r = 0.0;

	switch (node.type) {
	case TokenType::MINUS: r = -right; break;
	case TokenType::BANG: r = !right; break;
	default: assert(false);
	}

	stack.push_back(Value::Number(r));
}


