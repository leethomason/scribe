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
	std::vector<ASTStmtPtr> parseStmts();

private:
	Tokenizer& tok;

	bool check(TokenType type);
	bool match(const std::vector<TokenType>& types, Token& matched);	

	ASTStmtPtr statement();
	ASTStmtPtr declaration();
	ASTStmtPtr varDecl();
	ASTStmtPtr printStatement();
	ASTStmtPtr expressionStatement();
	ASTStmtPtr returnStatement();

	ASTExprPtr expression();
	ASTExprPtr equality();
	ASTExprPtr comparison();
	ASTExprPtr term();
	ASTExprPtr factor();
	ASTExprPtr unary();
	ASTExprPtr primary();
};
