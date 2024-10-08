#include "parser.h"

ASTNode* Parser::parse(Tokenizer& tokenizer)
{
	// FIXME: parses a line. Need to generalize.
	Token tok = tokenizer.getNext();
	if (tok.type == TokenType::eof)
		return nullptr;

	if (tok.type == TokenType::error) {
		fmt::print("Error parsing.\n");
		return nullptr;
	}

	// Variable declaration.
	// var x = y + 1
	// Variable assignment
	// x = 2 + 5 * 3
	if (tok.type == TokenType::var || tok.type == TokenType::identifier) {
		Token var;
		if (tok.type == TokenType::var) {
			var = tokenizer.getNext();
			if (var.type != TokenType::identifier) {
				fmt::print("Expected identifier\n");
				return nullptr;
			}
		}
		else {
			var = tok;
		}

		Token assign = tokenizer.getNext();
		if (assign.type != TokenType::assign) {
			fmt::print("Expected =\n");
			return nullptr;
		}

		ASTNode* e = expr(tokenizer);
		if (!e) return nullptr;

		if (tok.type == TokenType::var)
			return new DeclareVariableNode(var.value, e);
		else
			return new AssignmentNode(tok.value, e);
	}
	return nullptr;
}

ASTNode* Parser::expr(Tokenizer& izer)
{
	// x + (3 + y)
	// x + (3)

	Token tok = izer.getNext();
	if (tok.type == TokenType::leftParen) {
		ASTNode* node = expr(izer);
		
		Token right = izer.getNext();
		if (right.type != TokenType::rightParen) {
			fmt::print("Expected right paren.\n");
			return nullptr;
		}
		return node;
	}

	// FIXME: other types (bool, int, array, map, etc....)
	// 3 + x
	// x + 3
	// 3
	// x
	if (tok.type == TokenType::number) {
		ValueNode* left = new ValueNode(Value::Number(tok.dValue));
		Token op = izer.getNext();

		BinaryOpNode* node = new BinaryOpNode()
	}
}p