#pragma once

#include "token.h"
#include "ast.h"

#include <vector>

class Parser
{
public:
	ASTPtr parse(Tokenizer& tok) { return parseExpr(tok); }

	bool hasError() const { return !error.empty(); }
	const std::string& errorMessage() const { return error; }

private:
	std::string error;
	void setError(const std::string& e) {
		if (error.empty())
			error = e;
	}

	ASTPtr parsePrimaryExpr(Tokenizer& izer);
	ASTPtr parseMulExpr(Tokenizer& izer);
	ASTPtr parseExpr(Tokenizer& izer);
};
