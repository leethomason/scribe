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

	ASTExprPtr parseExpr() { return expression(); }
	//std::vector<ASTPtr> parseStmts();

private:
	Tokenizer& tok;

	bool check(TokenType type);
	bool match(const std::vector<TokenType>& types, Token& matched);	

	//ASTPtr statement();

	ASTExprPtr expression();
	ASTExprPtr equality();
	ASTExprPtr comparison();
	ASTExprPtr term();
	ASTExprPtr factor();
	ASTExprPtr unary();
	ASTExprPtr primary();
};
