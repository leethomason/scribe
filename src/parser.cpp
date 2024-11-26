#include "parser.h"
#include "ast.h"
#include "errorreporting.h"

/*
	Issues: 
		* handling EOL (no semicolon)
		* ternary! ternary is good.
		* note that peeking '{' or '}' terminates bunches of things - need to correctly get that in the grammar 

	program -> declaration* EOF

	declaration ->   varDecl 
				   | statement
	varDecl ->   "var" IDENTIFIER ( "=" expression )?
	           | "var" ":" IDENTIFIER ( "=" expression )?
	assignment -> IDENTIFIER "=" expression
	statement ->   exprStmt 
				 | ifStmt
				 | printStmt
				 | whileStmt
				 | returnStmt
				 | block
	exprStmt -> expr
	ifStmt -> "if" expression "{" statement ( "else" "{" statement )?	// Note that the "{" is not consumed by the ifStmt, but by the block
	printStmt -> "print" expr
	whileStmt -> "while" expression block
	returnStmt -> "return" expr
	block -> "{" declaration* "}"

	expr -> assignment
	assignment ->   IDENTIFIER "=" assignment
				  | equality
	logic_or -> logic_and ( "||" logic_and )*
	logic_and -> equality ( "&&" equality )*
	equality -> comparison ( ( "==" | "!=" ) comparison )*
	comparison -> term ( ( ">" | ">=" | "<" | "<=" ) term )*
	term -> factor ( ( "+" | "-" ) factor )*
	factor -> unary ( ( "*" | "/" ) unary )*
	unary -> ( "!" | "-" ) unary | primary
	primary -> NUMBER | STRING | "true" | "false" | "(" expr ")" | IDENTIFIER
*/

bool Parser::check(TokenType type) 
{
	if (tok.peek().type == type) {
		tok.get();
		return true;
	}
	return false;
}

bool Parser::check(TokenType type, Token& t)
{
	if (tok.peek().type == type) {
		t = tok.get();
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


std::vector<ASTStmtPtr>  Parser::parseStmts()
{
	std::vector<ASTStmtPtr> stmts;
	while (!tok.done()) {
		ASTStmtPtr stmt = declaration();
		if (stmt) {
			stmts.push_back(stmt);
		}
	}
	return stmts;
}

ASTStmtPtr Parser::declaration()
{
	if (check(TokenType::VAR))
		return varDecl();
	return statement();
}

ASTStmtPtr Parser::varDecl()
{
	Token t = tok.get();
	if (t.type != TokenType::IDENT) {
		ErrorReporter::report(ctxName, t.line, "Expected identifier");
		return nullptr;
	}

	if (check(TokenType::COLON)) {
		// "var" ":" IDENTIFIER ( "=" expression )?

		Token type = tok.get();
		if (type.type != TokenType::IDENT) {
			ErrorReporter::report(ctxName, t.line, "Expected type");
			return nullptr;
		}
		ValueType valueType = IdentToTypeName(type.lexeme);
		if (valueType == ValueType::tNone) {
			ErrorReporter::report(ctxName, t.line, "Unrecognized type");
			return nullptr;
		}

		ASTExprPtr expr = nullptr;
		if (check(TokenType::EQUAL)) {
			expr = expression();
		}

		return std::make_shared<ASTVarDeclStmtNode>(t.lexeme, valueType, expr);
	}
	else {
		// "var" IDENTIFIER ( "=" expression )?

		ASTExprPtr expr = nullptr;
		if (check(TokenType::EQUAL)) {
			expr = expression();
		}

		// Very simple duck typing rules!
		ValueType valueType = expr->duckType();
		if (valueType == ValueType::tNone) {
			ErrorReporter::report(ctxName, t.line, "Could not duck type");
			return nullptr;
		}

		return std::make_shared<ASTVarDeclStmtNode>(t.lexeme, valueType, expr);
	}
}

ASTStmtPtr Parser::statement()
{
	if (check(TokenType::LEFT_BRACE))
		return block();
	if (check(TokenType::RETURN))
		return returnStatement();
	if (check(TokenType::PRINT))
		return printStatement();
	if (check(TokenType::IF))
		return ifStatement();
	if (check(TokenType::WHILE))
		return whileStatement();
	return expressionStatement();
}

ASTStmtPtr Parser::printStatement()
{
	ASTExprPtr expr = expression();
	return std::make_shared<ASTPrintStmtNode>(expr);
}

ASTStmtPtr Parser::returnStatement()
{
	ASTExprPtr expr = expression();
	return std::make_shared<ASTReturnStmtNode>(expr);
}

ASTStmtPtr Parser::ifStatement()
{
	ASTExprPtr condition = expression();

	if (!check(TokenType::LEFT_BRACE)) {
		ErrorReporter::report(ctxName, tok.peek().line, "Expected '{'");
		return nullptr;
	}
	ASTStmtPtr thenBranch = block();
	REQUIRE(thenBranch);
	ASTStmtPtr elseBranch = nullptr;

	if (check(TokenType::ELSE)) {
		if (!check(TokenType::LEFT_BRACE)) {
			ErrorReporter::report(ctxName, tok.peek().line, "Expected '{'");
			return nullptr;
		}
		elseBranch = block();
	}

	return std::make_shared<ASTIfStmtNode>(condition, thenBranch, elseBranch);
}

ASTStmtPtr Parser::whileStatement()
{
	ASTExprPtr condition = expression();

	if (!check(TokenType::LEFT_BRACE)) {
		ErrorReporter::report(ctxName, tok.peek().line, "Expected '{'");
		return nullptr;
	}
	ASTStmtPtr body = block();
	REQUIRE(body);
	return std::make_shared<ASTWhileStmtNode>(condition, body);
}

ASTStmtPtr Parser::block()
{
	std::vector<ASTStmtPtr> stmts;

	while(!tok.done() && tok.peek().type != TokenType::RIGHT_BRACE) {
		stmts.push_back(declaration());
	}
	if (!check(TokenType::RIGHT_BRACE)) {
		ErrorReporter::report(ctxName, tok.peek().line, "Expected '}'");
		return nullptr;
	}
	return std::make_shared<ASTBlockStmtNode>(stmts);
}

ASTStmtPtr Parser::expressionStatement()
{
	ASTExprPtr expr = expression();
	return std::make_shared<ASTExprStmtNode>(expr);
}

ASTExprPtr Parser::expression()
{
	return assignment();
}

ASTExprPtr Parser::assignment()
{
	ASTExprPtr expr = logicalOR();

	Token t;
	if (check(TokenType::EQUAL, t)) {
		// The expression should be an l-value
		ASTExprPtr rValue = assignment();
		const IdentifierASTNode* ident = expr->asIdentifier();

		if (!ident) {
			ErrorReporter::report(ctxName, t.line, "Invalid assignment target, not an l-value");
			return nullptr;
		}
		return std::make_shared<AssignmentASTNode>(ident->name, rValue);
	}
	return expr;
}

ASTExprPtr Parser::logicalOR()
{
	ASTExprPtr expr = logicalAND();

	Token t;
	while (check(TokenType::LOGIC_OR, t)) {
		ASTExprPtr rhs = logicalAND();
		expr = std::make_shared<LogicalASTNode>(t.type, expr, rhs);
	}
	return expr;
}

ASTExprPtr Parser::logicalAND()
{
	ASTExprPtr expr = equality();

	Token t;
	while (check(TokenType::LOGIC_AND, t)) {
		ASTExprPtr rhs = equality();
		expr = std::make_shared<LogicalASTNode>(t.type, expr, rhs);
	}
	return expr;
}

ASTExprPtr Parser::equality()
{
	ASTExprPtr expr = comparison();
	Token t;
	while(match({TokenType::EQUAL_EQUAL, TokenType::BANG_EQUAL}, t)) {
		ASTExprPtr right = comparison();
		expr = std::make_shared<BinaryASTNode>(t.type, expr, right);
	}
	return expr;
}

ASTExprPtr Parser::comparison()
{
	ASTExprPtr expr = term();
	Token t;
	while (match({TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL}, t)) {
		ASTExprPtr right = term();
		expr = std::make_shared<BinaryASTNode>(t.type, expr, right);
	}
	return expr;
}

ASTExprPtr Parser::term()
{
	ASTExprPtr expr = factor();
	Token t;
	while (match({TokenType::PLUS, TokenType::MINUS}, t)) {
		ASTExprPtr right = factor();
		expr = std::make_shared<BinaryASTNode>(t.type, expr, right);
	}
	return expr;
}

ASTExprPtr Parser::factor()
{
	ASTExprPtr expr = unary();
	Token t;
	while (match({TokenType::MULT, TokenType::DIVIDE}, t)) {
		ASTExprPtr right = unary();
		expr = std::make_shared<BinaryASTNode>(t.type, expr, right);
	}
	return expr;
}

// Everything above is Binary, so just keep in the mind the conceptual switch
ASTExprPtr Parser::unary()
{
	Token t;
	if (match({TokenType::BANG, TokenType::MINUS}, t)) {
		ASTExprPtr right = unary();
		return std::make_shared<UnaryASTNode>(t.type, right);
	}
	return primary();
}

// NUMBER | STRING | identifier | "true" | "false" | "(" expr ")"
ASTExprPtr Parser::primary()
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
			ASTExprPtr expr = expression();
			if (!check(TokenType::RIGHT_PAREN)) {
				ErrorReporter::report(ctxName, t.line, "Expected ')'");
				return nullptr;
			}
			return expr;
		}
		default:
			ErrorReporter::report(ctxName, t.line, "Unexpected token");
	}
	return nullptr;
}
