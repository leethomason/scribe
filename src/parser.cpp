#include "parser.h"
#include "ast.h"
#include "errorreporting.h"

/*
	Issues: 
		* handling EOL (no semicolon)

	expr -> equality
	equality -> comparison ( ( "==" | "!=" ) comparison )*
	comparison -> term ( ( ">" | ">=" | "<" | "<=" ) term )*
	term -> factor ( ( "+" | "-" ) factor )*
	factor -> unary ( ( "*" | "/" ) unary )*
	unary -> ( "!" | "-" ) unary | primary
	primary -> NUMBER | STRING | identifier | "true" | "false" | "(" expr ")"
*/

bool Parser::check(TokenType type) 
{
	if (tok.peek().type == type) {
		tok.get();
		return true;
	}
	return false;
}

bool Parser::match(const std::vector<TokenType>& types, Token& t)
{
	Token peek = tok.peek();
	for(auto type : types) {
		if (peek.type == type) {
			t = tok.get();
			return true;
		}
	}
	return false;
}


ASTPtr Parser::expression()
{
	return equality();
}

ASTPtr Parser::equality()
{
	ASTPtr expr = comparison();
	Token t;
	while(match({TokenType::EQUAL_EQUAL, TokenType::BANG_EQUAL}, t)) {
		ASTPtr right = comparison();
		expr = std::make_shared<BinaryASTNode>(t.type, expr, right);
	}
	return expr;
}

ASTPtr Parser::comparison()
{
	ASTPtr expr = term();
	Token t;
	while (match({TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL}, t)) {
		ASTPtr right = term();
		expr = std::make_shared<BinaryASTNode>(t.type, expr, right);
	}
	return expr;
}

ASTPtr Parser::term()
{
	ASTPtr expr = factor();
	Token t;
	while (match({TokenType::PLUS, TokenType::MINUS}, t)) {
		ASTPtr right = factor();
		expr = std::make_shared<BinaryASTNode>(t.type, expr, right);
	}
	return expr;
}

ASTPtr Parser::factor()
{
	ASTPtr expr = unary();
	Token t;
	while (match({TokenType::MULT, TokenType::DIVIDE}, t)) {
		ASTPtr right = unary();
		expr = std::make_shared<BinaryASTNode>(t.type, expr, right);
	}
	return expr;
}

// Everything above is Binary, so just keep in the mind the conceptual switch
ASTPtr Parser::unary()
{
	Token t;
	if (match({TokenType::BANG, TokenType::MINUS}, t)) {
		ASTPtr right = unary();
		return std::make_shared<UnaryASTNode>(t.type, right);
	}
	return primary();
}

// NUMBER | STRING | identifier | "true" | "false" | "(" expr ")"
ASTPtr Parser::primary()
{
	Token t = tok.get();
	switch(t.type) {
		case TokenType::NUMBER:
			return std::make_shared<ValueASTNode>(Value::Number(t.dValue));
		case TokenType::STRING:
			return std::make_shared<ValueASTNode>(Value::String(t.lexeme));
		case TokenType::IDENT:
			return std::make_shared<IdentifierASTNode>(t.lexeme);
		case TokenType::TRUE:	
			return std::make_shared<ValueASTNode>(Value::Boolean(true));
		case TokenType::FALSE:
			return std::make_shared<ValueASTNode>(Value::Boolean(false));

		case TokenType::LEFT_PAREN:
		{
			ASTPtr expr = expression();
			if (!check(TokenType::RIGHT_PAREN)) {
				ErrorReporter::report("fixme", t.line, "Expected ')'");
				return nullptr;
			}
			return expr;
		}
		default:
			ErrorReporter::report("fixme", t.line, "Unexpected token");
	}
	return nullptr;
}


