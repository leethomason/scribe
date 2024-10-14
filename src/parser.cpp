#include "parser.h"
#include "ast.h"
#include "errorreporting.h"

/*
	PrimaryExpr :
		Number
		Name
		(Expr)

	MulExpr :
		PrimaryExpr ( * PrimaryExpr | / PrimaryExpr )*

	Expr :
		MulExpr ( + MulExpr | - MulExpr )*
*/

ASTPtr Parser::parsePrimaryExpr(Tokenizer& izer)
{
	Token t = izer.peek();
	if (t.type == TokenType::NUMBER) {
		izer.get();
		return std::make_shared<ValueASTNode>(Value::Number(t.dValue));
	}
	else if (t.type == TokenType::IDENT) {
		izer.get();
		return std::make_shared<IdentifierASTNode>(t.lexeme);
	}
	else if (t.type == TokenType::LEFT_PAREN) {
		izer.get();
		ASTPtr expr = parseExpr(izer);
		if (izer.get().type != TokenType::RIGHT_PAREN) {
			
			return nullptr;
		}
		return expr;
	}
	else {
		ErrorReporter::report("fixme", 0, "Expected number, identifier, or left paren.");
		return nullptr;
	}
}

ASTPtr Parser::parseMulExpr(Tokenizer& izer)
{
	ASTPtr expr = parsePrimaryExpr(izer);
	if (!expr)
		return nullptr;

	Token t = izer.peek();
	while (t.type == TokenType::MULT || t.type == TokenType::DIVIDE) {
		izer.get();
		ASTPtr right = parsePrimaryExpr(izer);
		if (!right)
			return nullptr;

		// Note the recursive use of 'expr'
		expr = std::make_shared<BinaryASTNode>(t.type, expr, right);
		t = izer.peek();
	}
	return expr;
}

ASTPtr Parser::parseExpr(Tokenizer& izer)
{
	ASTPtr expr = parseMulExpr(izer);
	Token t = izer.peek();
	while (t.type == TokenType::PLUS || t.type == TokenType::MINUS) {
		izer.get();
		ASTPtr right = parseMulExpr(izer);

		// Note the recursive use of 'expr'
		expr = std::make_shared<BinaryASTNode>(t.type, expr, right);
		t = izer.peek();
	}
	return expr;
}

