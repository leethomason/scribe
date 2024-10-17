#pragma once

#include "token.h"
#include "ast.h"

#include <vector>

/* 
* The Parser produces the AST
*/
class Parser
{
public:
	Parser(Tokenizer& tok) : tok(tok) {}
	ASTPtr parse() { return expression(); }

private:
	Tokenizer& tok;

	bool check(TokenType type);
	bool match(const std::vector<TokenType>& types, Token& matched);	

	ASTPtr expression();
	ASTPtr equality();
	ASTPtr comparison();
	ASTPtr term();
	ASTPtr factor();
	ASTPtr unary();
	ASTPtr primary();
};
