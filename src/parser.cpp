#include "parser.h"
#include "ast.h"

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
	Token t = izer.peekNext();
	if (t.type == TokenType::number) {
		izer.getNext();
		return std::make_shared<ValueASTNode>(Value::Number(t.dValue));
	}
	else if (t.type == TokenType::identifier) {
		izer.getNext();
		return std::make_shared<IdentifierASTNode>(t.value);
	}
	else if (t.type == TokenType::leftParen) {
		izer.getNext();
		ASTPtr expr = parseExpr(izer);
		if (izer.getNext().type != TokenType::rightParen) {
			setError("Expected right paren.");
			return nullptr;
		}
		return expr;
	}
	else {
		setError("Expected number, identifier, or left paren.");
		return nullptr;
	}
}

ASTPtr Parser::parseMulExpr(Tokenizer& izer)
{
	ASTPtr expr = parsePrimaryExpr(izer);
	if (!expr)
		return nullptr;

	Token t = izer.peekNext();
	while (t.type == TokenType::multiply || t.type == TokenType::divide) {
		izer.getNext();
		ASTPtr right = parsePrimaryExpr(izer);
		if (!right)
			return nullptr;

		// Note the recursive use of 'expr'
		expr = std::make_shared<BinaryASTNode>(t.type, expr, right);
		t = izer.peekNext();
	}
	return expr;
}

ASTPtr Parser::parseExpr(Tokenizer& izer)
{
	ASTPtr expr = parseMulExpr(izer);
	Token t = izer.peekNext();
	while (t.type == TokenType::plus || t.type == TokenType::minus) {
		izer.getNext();
		ASTPtr right = parseMulExpr(izer);

		// Note the recursive use of 'expr'
		expr = std::make_shared<BinaryASTNode>(t.type, expr, right);
		t = izer.peekNext();
	}
	return expr;
}

