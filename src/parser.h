#pragma once

#include "token.h"
#include "ast.h"

class Parser
{
public:
	ASTNode* parse(Tokenizer& tok);

private:
	ASTNode* expr(Tokenizer& tok);
};