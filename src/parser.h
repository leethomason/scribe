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
	Parser(Tokenizer& tok, const std::string& ctxName) : tok(tok), ctxName(ctxName) {}

	ASTExprPtr parseExpr() { return expression(); }
	std::vector<ASTStmtPtr> parseStmts();

private:
	Tokenizer& tok;
	std::string ctxName;

	bool check(TokenType type);
	bool check(TokenType type, Token& matched);	// returns the token, with the line #, etc.

	bool match(const std::vector<TokenType>& types, Token& matched);	

	ASTStmtPtr statement();
	ASTStmtPtr declaration();
	ASTStmtPtr varDecl();
	ASTStmtPtr printStatement();
	ASTStmtPtr expressionStatement();
	ASTStmtPtr returnStatement();
	ASTStmtPtr ifStatement();
	ASTStmtPtr whileStatement();
	ASTStmtPtr forStatement();
	ASTStmtPtr block();				// consumes final brace but not opening one

	ASTExprPtr expression();
	ASTExprPtr assignment();
	ASTExprPtr equality();
	ASTExprPtr logicalOR();
	ASTExprPtr logicalAND();
	ASTExprPtr comparison();
	ASTExprPtr term();
	ASTExprPtr factor();
	ASTExprPtr unary();
	ASTExprPtr primary();
};
