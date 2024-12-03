#pragma once

#include "bytecode.h"
#include "value.h"
#include "token.h"

#include <map>
#include <string>
#include <vector>

#define DEBUG_AST_CREATION() 0
#define DEBUG_AST_VISIT() 0

#if DEBUG_AST_CREATION()
#   define LOG_AST(x) fmt::print("{}\n", #x);
#else
#   define LOG_AST(x)
#endif

#if DEBUG_AST_VISIT()
#   define LOG_AST_VISIT(x, d) fmt::print("visit: {: >{}}{}\n", "", d*2, #x);
#else
#   define LOG_AST_VISIT(x, d)
#endif

class ASTStmtNode;
class ASTExprStmt;
class ASTReturnStmt;
class ASTBlockStmt;
class ASTVarDeclStmt;
class ASTIfStmt;
class ASTWhileStmt;

using ASTStmtPtr = std::shared_ptr<ASTStmtNode>;

class ASTExprNode;
class ASTValueExpr;
class ASTAssignmentExpr;
class ASTIdentifierExpr;
class ASTBinaryExpr;
class ASTUnaryExpr;
class ASTLogicalExpr;
class ASTCallExpr;

using ASTExprPtr = std::shared_ptr<ASTExprNode>;

// -------- Statements ----------

class ASTStmtVisitor
{
public:
	virtual void visit(const ASTExprStmt&, int depth) = 0;
    virtual void visit(const ASTReturnStmt&, int depth) = 0;
	virtual void visit(const ASTBlockStmt&, int depth) = 0;
    virtual void visit(const ASTVarDeclStmt&, int depth) = 0;
	virtual void visit(const ASTIfStmt&, int depth) = 0;
    virtual void visit(const ASTWhileStmt&, int depth) = 0;
};

class ASTStmtNode {
    public:
	virtual ~ASTStmtNode() = default;
	virtual void accept(ASTStmtVisitor& visitor, int depth) const = 0;
};

class ASTExprStmt : public ASTStmtNode
{
public:
	ASTExprStmt(ASTExprPtr expr) : expr(expr) {
        LOG_AST(ASTExprStmt);
    }
    virtual void accept(ASTStmtVisitor& visitor, int depth) const override { 
        LOG_AST_VISIT(ASTExprStmt, depth);
        visitor.visit(*this, depth);  
    }

	ASTExprPtr expr;
};

class ASTReturnStmt : public ASTStmtNode
{
public:
    ASTReturnStmt(ASTExprPtr expr) : expr(expr) {
        LOG_AST(ASTReturnStmt);
    }
    virtual void accept(ASTStmtVisitor& visitor, int depth) const override { 
        LOG_AST_VISIT(ASTReturnStmt, depth);
        visitor.visit(*this, depth); 
    }

    ASTExprPtr expr;
};

class ASTWhileStmt : public ASTStmtNode
{
public:
    ASTWhileStmt(ASTExprPtr condition, ASTStmtPtr body) : condition(condition), body(body) {
        LOG_AST(ASTWhileStmt);
    }
    virtual void accept(ASTStmtVisitor& visitor, int depth) const override { 
        LOG_AST_VISIT(ASTWhileStmt, depth);
        visitor.visit(*this, depth); 
    }

    ASTExprPtr condition;
    ASTStmtPtr body;
};

class ASTBlockStmt : public ASTStmtNode
{
public:
	ASTBlockStmt(const std::vector<ASTStmtPtr>& stmts) : stmts(stmts) {
        LOG_AST(ASTBlockStmt);
    }
	virtual void accept(ASTStmtVisitor& visitor, int depth) const override { 
        LOG_AST_VISIT(ASTBlockStmt, depth);
        visitor.visit(*this, depth); 
    }
	
    std::vector<ASTStmtPtr> stmts;
};

class ASTIfStmt : public ASTStmtNode
{
public:
	ASTIfStmt(ASTExprPtr condition, ASTStmtPtr thenBranch, ASTStmtPtr elseBranch) : condition(condition), thenBranch(thenBranch), elseBranch(elseBranch) {
        REQUIRE(condition);
        REQUIRE(thenBranch);
        LOG_AST(ASTIfStmt);
    }
	virtual void accept(ASTStmtVisitor& visitor, int depth) const override { 
        LOG_AST_VISIT(ASTIfStmt, depth);
        visitor.visit(*this, depth);
    }

	ASTExprPtr condition;
	ASTStmtPtr thenBranch;
	ASTStmtPtr elseBranch;
};

class ASTVarDeclStmt : public ASTStmtNode
{
public:
    ASTVarDeclStmt(const std::string& name, ValueType valueType, ASTExprPtr expr) : name(name), valueType(valueType), expr(expr) {
        LOG_AST(ASTVarDeclStmt);
    }
    virtual void accept(ASTStmtVisitor& visitor, int depth) const override { 
        LOG_AST_VISIT(ASTVarDeclStmt, depth);
        visitor.visit(*this, depth);
    }

    std::string name;
    ValueType valueType;
    ASTExprPtr expr;
};

// -------- Expressions ----------

class ASTExprVisitor
{
public:
    virtual void visit(const ASTAssignmentExpr&, int depth) = 0;
    virtual void visit(const ASTValueExpr&, int depth) = 0;
    virtual void visit(const ASTIdentifierExpr&, int depth) = 0;
    virtual void visit(const ASTBinaryExpr&, int depth) = 0;
    virtual void visit(const ASTUnaryExpr&, int depth) = 0;
    virtual void visit(const ASTLogicalExpr&, int depth) = 0;
    virtual void visit(const ASTCallExpr&, int depth) = 0;
};

// Abstract syntax tree (AST) nodes
class ASTExprNode {
public:
    virtual ~ASTExprNode() = default;
    virtual void accept(ASTExprVisitor& visitor, int depth) const = 0;

    virtual ValueType duckType() const {
        return ValueType();
    }
    virtual const ASTIdentifierExpr* asIdentifier() { return nullptr; }
};

class ASTValueExpr : public ASTExprNode
{
public:
    ASTValueExpr(const Value& value) : value(value) {
        LOG_AST(ASTValueExpr);
    }

    virtual void accept(ASTExprVisitor& visitor, int depth) const override { 
        LOG_AST_VISIT(ASTValueExpr, depth);
        visitor.visit(*this, depth);
    }

    virtual ValueType duckType() const override {
        return value.type;
    }

    Value value;
};

class ASTAssignmentExpr : public ASTExprNode
{
public:
	ASTAssignmentExpr(const std::string& name, ASTExprPtr right) : name(name), right(right) {
        LOG_AST(ASTAssignmentExpr);
    }

    virtual void accept(ASTExprVisitor& visitor, int depth) const override { 
        LOG_AST_VISIT(ASTAssignmentExpr, depth);
        visitor.visit(*this, depth);
    }

	std::string name;
	ASTExprPtr right;
};

class ASTIdentifierExpr : public ASTExprNode
{ 
public:
	ASTIdentifierExpr(const std::string& name) : name(name) {
        LOG_AST(ASTIdentifierExpr);
    }
    virtual void accept(ASTExprVisitor& visitor, int depth) const override { 
        LOG_AST_VISIT(ASTIdentifierExpr, depth);
        visitor.visit(*this, depth); 
    }
    virtual const ASTIdentifierExpr* asIdentifier() override { return this; }

    std::string name;
};

class ASTBinaryExpr : public ASTExprNode
{
public:
    ASTBinaryExpr(TokenType type, ASTExprPtr left, ASTExprPtr right) : type(type) {
        this->left = left;
        this->right = right;
        LOG_AST(ASTBinaryExpr);
    }
    virtual void accept(ASTExprVisitor& visitor, int depth) const override { 
        LOG_AST_VISIT(ASTBinaryExpr, depth);
        visitor.visit(*this, depth); 
    }

    TokenType type;
    ASTExprPtr left;
    ASTExprPtr right;
};

class ASTUnaryExpr : public ASTExprNode
{
public:
    ASTUnaryExpr(TokenType type,ASTExprPtr right) : type(type) {
        this->right = right;
        LOG_AST(ASTUnaryExpr);
    }
    virtual void accept(ASTExprVisitor& visitor, int depth) const override { 
        LOG_AST_VISIT(ASTUnaryExpr, depth);
        visitor.visit(*this, depth); 
    }

    TokenType type;
    ASTExprPtr right;
};

class ASTCallExpr : public ASTExprNode
{
public:
    ASTCallExpr(ASTExprPtr callee, Token paren, const std::vector<ASTExprPtr>& arguments) :
        callee(callee), paren(paren), arguments(arguments) {}

    virtual void accept(ASTExprVisitor& visitor, int depth) const override {
        LOG_AST_VISIT(ASTCallExpr, depth);
        visitor.visit(*this, depth);
    }

    ASTExprPtr callee;
    Token paren;
    std::vector<ASTExprPtr> arguments;
};

class ASTLogicalExpr : public ASTExprNode
{
    public:
	ASTLogicalExpr(TokenType type, ASTExprPtr left, ASTExprPtr right) : type(type) {
		this->left = left;
		this->right = right;
		LOG_AST(ASTLogicalExpr);
	}
	virtual void accept(ASTExprVisitor& visitor, int depth) const override { 
        LOG_AST_VISIT(ASTLogicalExpr, depth);
        visitor.visit(*this, depth); 
    }

	TokenType type;
	ASTExprPtr left;
	ASTExprPtr right;
};
