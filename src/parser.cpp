#include "parser.h"
#include "ast.h"
#include "errorreporting.h"

/*
	program -> declaration* EOF

	declaration ->   funcDecl
				   | varDecl 
				   | statement
	funcDecl ->  "func" function
	function -> IDENTIFIER "(" parameters? ")" ( ":" IDENTIFIER )? block
	parameters -> IDENTIFIER ( "," IDENTIFIER )* 
	varDecl ->   "var" IDENTIFIER "=" expression						// must be able to duck type
	           | "var" IDENTIFIER ":" IDENTIFIER ( "=" expression )?
	assignment ->   IDENTIFIER "=" expression
	statement ->   exprStmt 
				 | forStmt
				 | ifStmt
				 | whileStmt
				 | returnStmt
				 | block
	exprStmt -> expr
	forStmt -> "for" "(" (varDecl | exprStmt | ";" ) 
				expression? ";"
				expression? 
				block
	ifStmt -> "if" expression "{" statement ( "else" "{" statement )?	// Note that the "{" is not consumed by the ifStmt, but by the block
	whileStmt -> "while" expression block
	returnStmt -> "return" expr
	block -> "{" declaration* "}"

	expression -> assignment
	assignment ->   IDENTIFIER "=" assignment
				  | equality
	logic_or -> logic_and ( "||" logic_and )*
	logic_and -> equality ( "&&" equality )*
	equality -> comparison ( ( "==" | "!=" ) comparison )*
	comparison -> term ( ( ">" | ">=" | "<" | "<=" ) term )*
	term -> factor ( ( "+" | "-" ) factor )*
	factor -> unary ( ( "*" | "/" ) unary )*
	unary -> ( "!" | "-" ) unary | call
	call -> primary ( "(" arguments? ")" )*
	primary -> NUMBER | STRING | "true" | "false" | "(" expression ")" | IDENTIFIER
	arguments -> expression ( "," expression )*
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

bool Parser::peek(TokenType type)
{
	if (tok.peek().type == type) {
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
	if (check(TokenType::FUNC))
		return funcDecl();
	if (check(TokenType::VAR))
		return varDecl();
	return statement();
}

ASTStmtPtr Parser::funcDecl()
{
	Token name = tok.get();
	if (name.type != TokenType::IDENT) {
		ErrorReporter::report(ctxName, name.line, "Expected function name");
		return nullptr;
	}
	if (!check(TokenType::LEFT_PAREN)) {
		ErrorReporter::report(ctxName, tok.peek().line, "Expected '('");
		return nullptr;
	}

	// Params
	std::vector<Param> params;
	if (!peek(TokenType::RIGHT_PAREN)) {
		do {
			Token param = tok.get();
			if (param.type != TokenType::IDENT) {
				ErrorReporter::report(ctxName, param.line, "Expected parameter name");
				return nullptr;
			}
			if (!check(TokenType::COLON)) {
				ErrorReporter::report(ctxName, tok.peek().line, "Expected ':'");
				return nullptr;
			}
			Token type;
			if (!check({ TokenType::IDENT }, type)) {
				ErrorReporter::report(ctxName, tok.peek().line, "Expected type");
				return nullptr;
			}
			ValueType vt = ValueType::fromTypeName(type.lexeme);
			if (vt == ValueType()) {
				ErrorReporter::report(ctxName, type.line, "Unrecognized type");
				return nullptr;
			}
			params.push_back(Param{ param.lexeme, vt });
		} while (check(TokenType::COMMA));
	}
	if (!check(TokenType::RIGHT_PAREN)) {
		ErrorReporter::report(ctxName, tok.peek().line, "Expected ')'");
		return nullptr;
	}

	ValueType rcType;
	if (!check(TokenType::COLON)) {
		if (!check(TokenType::IDENT)) {
			ErrorReporter::report(ctxName, tok.peek().line, "Expected return type");
			return nullptr;
		}
		rcType = ValueType::fromTypeName(tok.get().lexeme);
		if (rcType == ValueType()) {
			ErrorReporter::report(ctxName, name.line, "Unrecognized return type");
			return nullptr;
		}
	}

	ASTStmtPtr b = block();
	return std::make_shared<ASTFuncDeclStmt>(name.lexeme, params, rcType, b);
}

ASTStmtPtr Parser::varDecl()
{
	// There's a lot of possibilites here!
	// var a = 0			// duck typed
	// var a: num = 0		// declared type
	// var a = [0, 1, 2]	// duck typed
	// var a: num[] = []	// declared type
	// the "var" has already been read, so we know where we are

	Token t = tok.get();
	if (t.type != TokenType::IDENT) {
		ErrorReporter::report(ctxName, t.line, "Expected identifier");
		return nullptr;
	}

	if (check(TokenType::COLON)) {
		// var a: num = 0		// declared type
		// var a: num[] = []	// declared type
		// var a: num
		// var a: num[]

		Token type;
		// Read type
		if (!check({ TokenType::IDENT }, type)) {
			ErrorReporter::report(ctxName, t.line, "Expected type");
			return nullptr;
		}
		ValueType valueType = ValueType::fromTypeName(type.lexeme);
		if (valueType == ValueType()) {
			ErrorReporter::report(ctxName, t.line, "Unrecognized type");
			return nullptr;
		}
		// Scalar, list, map
		if (check(TokenType::LEFT_BRACKET)) {
			if (!check(TokenType::RIGHT_BRACKET)) {
				ErrorReporter::report(ctxName, t.line, "Expected ']'");
				return nullptr;
			}
			valueType.layout = Layout::tList;
		}

		ASTExprPtr expr = nullptr;
		if (check(TokenType::EQUAL)) {
			expr = expression();
		}
		return std::make_shared<ASTVarDeclStmt>(t.lexeme, valueType, expr);
	}
	else {
		// "var" IDENTIFIER ( "=" expression )?

		ASTExprPtr expr = nullptr;
		if (check(TokenType::EQUAL)) {
			expr = expression();
		}

		// Very simple duck typing rules!
		ValueType valueType = expr->duckType();
		if (valueType == ValueType()) {
			ErrorReporter::report(ctxName, t.line, "Could not duck type");
			return nullptr;
		}

		return std::make_shared<ASTVarDeclStmt>(t.lexeme, valueType, expr);
	}
	/*
	assert(false);	// logic isn't correct, something isn't implemented.
					// this method is huge and needs to be simplified
	return nullptr;
	*/
}

ASTStmtPtr Parser::statement()
{
	if (check(TokenType::LEFT_BRACE))
		return block();
	if (check(TokenType::FOR))
		return forStatement();
	if (check(TokenType::RETURN))
		return returnStatement();
	if (check(TokenType::IF))
		return ifStatement();
	if (check(TokenType::WHILE))
		return whileStatement();
	return expressionStatement();
}

ASTStmtPtr Parser::returnStatement()
{
	ASTExprPtr expr = expression();
	return std::make_shared<ASTReturnStmt>(expr);
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

	return std::make_shared<ASTIfStmt>(condition, thenBranch, elseBranch);
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
	return std::make_shared<ASTWhileStmt>(condition, body);
}

ASTStmtPtr Parser::forStatement()
{
	// "for" (varDecl | exprStmt | ";" ) 
	//			expression? ";"
	//			expression? 
	//			block

	ASTStmtPtr init = nullptr;
	ASTExprPtr condition = nullptr;
	ASTExprPtr increment = nullptr;
	ASTStmtPtr body = nullptr;

	if (!check(TokenType::SEMICOLON)) {
		if (check(TokenType::VAR)) {
			init = varDecl();
		}
		else {
			init = expressionStatement();
		}
		if (!check(TokenType::SEMICOLON)) {
			ErrorReporter::report(ctxName, tok.peek().line, "Expected ';'");
			return nullptr;
		}
	}

	if (!check(TokenType::SEMICOLON)) {
		condition = expression();
		if (!check(TokenType::SEMICOLON)) {
			ErrorReporter::report(ctxName, tok.peek().line, "Expected ';'");
			return nullptr;
		}
	}

	if (!check(TokenType::LEFT_BRACE)) {
		increment = expression();
	}

	if (!check(TokenType::LEFT_BRACE)) {
		ErrorReporter::report(ctxName, tok.peek().line, "Expected '{'");
		return nullptr;
	}

	body = block();
	REQUIRE(body);

	// so this is:
	/*
		{												// outer block
			var i = 0;			// init
			while (i < 10) {	// condition			// inner: coniditon + block
				print i;		// body 
				i = i + 1;		// increment
			}
		}
	*/

	// Start with the inner block.
	std::vector<ASTStmtPtr> inner;
	inner.push_back(body);
	inner.push_back(std::make_shared<ASTExprStmt>(increment));
	std::shared_ptr<ASTBlockStmt> innerBlock = std::make_shared<ASTBlockStmt>(inner);

	if (!condition)
		condition = std::make_shared<ASTValueExpr>(Value::Boolean(true));
	std::shared_ptr<ASTWhileStmt> whileStmt = std::make_shared<ASTWhileStmt>(condition, innerBlock);

	// Now the outer block
	std::vector<ASTStmtPtr> outer;
	if (init)
		outer.push_back(init);
	outer.push_back(whileStmt);

	std::shared_ptr<ASTBlockStmt> outerBlock = std::make_shared<ASTBlockStmt>(outer);
	return outerBlock;
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
	return std::make_shared<ASTBlockStmt>(stmts);
}

ASTStmtPtr Parser::expressionStatement()
{
	ASTExprPtr expr = expression();
	return std::make_shared<ASTExprStmt>(expr);
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
		const ASTIdentifierExpr* ident = expr->asIdentifier();

		if (!ident) {
			ErrorReporter::report(ctxName, t.line, "Invalid assignment target, not an l-value");
			return nullptr;
		}
		return std::make_shared<ASTAssignmentExpr>(ident->name, rValue);
	}
	return expr;
}

ASTExprPtr Parser::logicalOR()
{
	ASTExprPtr expr = logicalAND();

	Token t;
	while (check(TokenType::LOGIC_OR, t)) {
		ASTExprPtr rhs = logicalAND();
		expr = std::make_shared<ASTLogicalExpr>(t.type, expr, rhs);
	}
	return expr;
}

ASTExprPtr Parser::logicalAND()
{
	ASTExprPtr expr = equality();

	Token t;
	while (check(TokenType::LOGIC_AND, t)) {
		ASTExprPtr rhs = equality();
		expr = std::make_shared<ASTLogicalExpr>(t.type, expr, rhs);
	}
	return expr;
}

ASTExprPtr Parser::equality()
{
	ASTExprPtr expr = comparison();
	Token t;
	while(match({TokenType::EQUAL_EQUAL, TokenType::BANG_EQUAL}, t)) {
		ASTExprPtr right = comparison();
		expr = std::make_shared<ASTBinaryExpr>(t.type, expr, right);
	}
	return expr;
}

ASTExprPtr Parser::comparison()
{
	ASTExprPtr expr = term();
	Token t;
	while (match({TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL}, t)) {
		ASTExprPtr right = term();
		expr = std::make_shared<ASTBinaryExpr>(t.type, expr, right);
	}
	return expr;
}

ASTExprPtr Parser::term()
{
	ASTExprPtr expr = factor();
	Token t;
	while (match({TokenType::PLUS, TokenType::MINUS}, t)) {
		ASTExprPtr right = factor();
		expr = std::make_shared<ASTBinaryExpr>(t.type, expr, right);
	}
	return expr;
}

ASTExprPtr Parser::factor()
{
	ASTExprPtr expr = unary();
	Token t;
	while (match({TokenType::MULT, TokenType::DIVIDE}, t)) {
		ASTExprPtr right = unary();
		expr = std::make_shared<ASTBinaryExpr>(t.type, expr, right);
	}
	return expr;
}

// Everything above is Binary, so just keep in the mind the conceptual switch
ASTExprPtr Parser::unary()
{
	Token t;
	if (match({TokenType::BANG, TokenType::MINUS}, t)) {
		ASTExprPtr right = unary();
		return std::make_shared<ASTUnaryExpr>(t.type, right);
	}
	return call();
}

ASTExprPtr Parser::call()
{
	ASTExprPtr expr = primary();

	while (true) {
		if (check(TokenType::LEFT_PAREN)) {
			expr = finishCall(expr);
		}
		else {
			break;
		}
	}
	return expr;
}

ASTExprPtr Parser::finishCall(ASTExprPtr expr)
{
	std::vector<ASTExprPtr> arguments;
	if (tok.peek().type != TokenType::RIGHT_PAREN) {
		do {
			arguments.push_back(expression());
		} while (check(TokenType::COMMA));
	}
	Token t;
	if (!check(TokenType::RIGHT_PAREN, t)) {
		ErrorReporter::report(ctxName, t.line, "Expected ')' after arguments");
		return nullptr;
	}
	// FIXME: maximum argument count
	return std::make_shared<ASTCallExpr>(expr, t, arguments);
}

// NUMBER | STRING | identifier | "true" | "false" | "(" expr ")"
ASTExprPtr Parser::primary()
{
	Token t = tok.get();
	switch(t.type) {
		case TokenType::NUMBER:
			return std::make_shared<ASTValueExpr>(Value::Number(t.dValue));
		case TokenType::STRING:
			return std::make_shared<ASTValueExpr>(Value::String(t.lexeme));
		case TokenType::IDENT:
			return std::make_shared<ASTIdentifierExpr>(t.lexeme);
		case TokenType::TRUE:	
			return std::make_shared<ASTValueExpr>(Value::Boolean(true));
		case TokenType::FALSE:
			return std::make_shared<ASTValueExpr>(Value::Boolean(false));

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
