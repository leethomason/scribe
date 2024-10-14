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
	ASTPtr parse(Tokenizer& tok) { return parseExpr(tok); }

private:
	ASTPtr parsePrimaryExpr(Tokenizer& izer);
	ASTPtr parseMulExpr(Tokenizer& izer);
	ASTPtr parseExpr(Tokenizer& izer);
};
