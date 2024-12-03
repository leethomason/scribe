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
class ASTExprStmtNode;
class ASTPrintStmtNode;
class ASTReturnStmtNode;
class ASTBlockStmtNode;
class ASTVarDeclStmtNode;
class ASTIfStmtNode;
class ASTWhileStmtNode;

using ASTStmtPtr = std::shared_ptr<ASTStmtNode>;

class ASTExprNode;
class ValueASTNode;
class AssignmentASTNode;
class IdentifierASTNode;
class BinaryASTNode;
class UnaryASTNode;
class LogicalASTNode;
class CallASTNode;

using ASTExprPtr = std::shared_ptr<ASTExprNode>;

// -------- Statements ----------

class ASTStmtVisitor
{
public:
	virtual void visit(const ASTExprStmtNode&, int depth) = 0;
	virtual void visit(const ASTPrintStmtNode&, int depth) = 0;
    virtual void visit(const ASTReturnStmtNode&, int depth) = 0;
	virtual void visit(const ASTBlockStmtNode&, int depth) = 0;
    virtual void visit(const ASTVarDeclStmtNode&, int depth) = 0;
	virtual void visit(const ASTIfStmtNode&, int depth) = 0;
    virtual void visit(const ASTWhileStmtNode&, int depth) = 0;
};

class ASTStmtNode {
    public:
	virtual ~ASTStmtNode() = default;
	virtual void accept(ASTStmtVisitor& visitor, int depth) const = 0;
};

class ASTExprStmtNode : public ASTStmtNode
{
public:
	ASTExprStmtNode(ASTExprPtr expr) : expr(expr) {
        LOG_AST(ASTExprStmtNode);
    }
    virtual void accept(ASTStmtVisitor& visitor, int depth) const override { 
        LOG_AST_VISIT(ASTExprStmtNode, depth);
        visitor.visit(*this, depth);  
    }

	ASTExprPtr expr;
};

class ASTPrintStmtNode : public ASTStmtNode
{
public:
	ASTPrintStmtNode(ASTExprPtr expr) : expr(expr) {
        LOG_AST(ASTPrintStmtNode);
    }
    virtual void accept(ASTStmtVisitor& visitor, int depth) const override {         
        LOG_AST_VISIT(ASTPrintStmtNode, depth);
        visitor.visit(*this, depth); 
    }

	ASTExprPtr expr;
};

class ASTReturnStmtNode : public ASTStmtNode
{
public:
    ASTReturnStmtNode(ASTExprPtr expr) : expr(expr) {
        LOG_AST(ASTReturnStmtNode);
    }
    virtual void accept(ASTStmtVisitor& visitor, int depth) const override { 
        LOG_AST_VISIT(ASTReturnStmtNode, depth);
        visitor.visit(*this, depth); 
    }

    ASTExprPtr expr;
};

class ASTWhileStmtNode : public ASTStmtNode
{
public:
    ASTWhileStmtNode(ASTExprPtr condition, ASTStmtPtr body) : condition(condition), body(body) {
        LOG_AST(ASTWhileStmtNode);
    }
    virtual void accept(ASTStmtVisitor& visitor, int depth) const override { 
        LOG_AST_VISIT(ASTWhileStmtNode, depth);
        visitor.visit(*this, depth); 
    }

    ASTExprPtr condition;
    ASTStmtPtr body;
};

class ASTBlockStmtNode : public ASTStmtNode
{
public:
	ASTBlockStmtNode(const std::vector<ASTStmtPtr>& stmts) : stmts(stmts) {
        LOG_AST(ASTBlockStmtNode);
    }
	virtual void accept(ASTStmtVisitor& visitor, int depth) const override { 
        LOG_AST_VISIT(ASTBlockStmtNode, depth);
        visitor.visit(*this, depth); 
    }
	
    std::vector<ASTStmtPtr> stmts;
};

class ASTIfStmtNode : public ASTStmtNode
{
public:
	ASTIfStmtNode(ASTExprPtr condition, ASTStmtPtr thenBranch, ASTStmtPtr elseBranch) : condition(condition), thenBranch(thenBranch), elseBranch(elseBranch) {
        REQUIRE(condition);
        REQUIRE(thenBranch);
        LOG_AST(ASTIfStmtNode);
    }
	virtual void accept(ASTStmtVisitor& visitor, int depth) const override { 
        LOG_AST_VISIT(ASTIfStmtNode, depth);
        visitor.visit(*this, depth);
    }

	ASTExprPtr condition;
	ASTStmtPtr thenBranch;
	ASTStmtPtr elseBranch;
};

class ASTVarDeclStmtNode : public ASTStmtNode
{
public:
    ASTVarDeclStmtNode(const std::string& name, ValueType valueType, ASTExprPtr expr) : name(name), valueType(valueType), expr(expr) {
        LOG_AST(ASTVarDeclStmtNode);
    }
    virtual void accept(ASTStmtVisitor& visitor, int depth) const override { 
        LOG_AST_VISIT(ASTVarDeclStmtNode, depth);
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
    virtual void visit(const AssignmentASTNode&, int depth) = 0;
    virtual void visit(const ValueASTNode&, int depth) = 0;
    virtual void visit(const IdentifierASTNode&, int depth) = 0;
    virtual void visit(const BinaryASTNode&, int depth) = 0;
    virtual void visit(const UnaryASTNode&, int depth) = 0;
    virtual void visit(const LogicalASTNode&, int depth) = 0;
    virtual void visit(const CallASTNode&, int depth) = 0;
};

// Abstract syntax tree (AST) nodes
class ASTExprNode {
public:
    virtual ~ASTExprNode() = default;
    virtual void accept(ASTExprVisitor& visitor, int depth) const = 0;

    virtual ValueType duckType() const {
        return ValueType();
    }
    virtual const IdentifierASTNode* asIdentifier() { return nullptr; }
};

class ValueASTNode : public ASTExprNode
{
public:
    ValueASTNode(const Value& value) : value(value) {
        LOG_AST(ValueASTNode);
    }

    virtual void accept(ASTExprVisitor& visitor, int depth) const override { 
        LOG_AST_VISIT(ValueASTNode, depth);
        visitor.visit(*this, depth);
    }

    virtual ValueType duckType() const override {
        return value.type;
    }

    Value value;
};

class AssignmentASTNode : public ASTExprNode
{
public:
	AssignmentASTNode(const std::string& name, ASTExprPtr right) : name(name), right(right) {
        LOG_AST(AssignmentASTNode);
    }

    virtual void accept(ASTExprVisitor& visitor, int depth) const override { 
        LOG_AST_VISIT(AssignmentASTNode, depth);
        visitor.visit(*this, depth);
    }

	std::string name;
	ASTExprPtr right;
};

class IdentifierASTNode : public ASTExprNode
{ 
public:
	IdentifierASTNode(const std::string& name) : name(name) {
        LOG_AST(IdentifierASTNode);
    }
    virtual void accept(ASTExprVisitor& visitor, int depth) const override { 
        LOG_AST_VISIT(IdentifierASTNode, depth);
        visitor.visit(*this, depth); 
    }
    virtual const IdentifierASTNode* asIdentifier() override { return this; }

    std::string name;
};

class BinaryASTNode : public ASTExprNode
{
public:
    BinaryASTNode(TokenType type, ASTExprPtr left, ASTExprPtr right) : type(type) {
        this->left = left;
        this->right = right;
        LOG_AST(BinaryASTNode);
    }
    virtual void accept(ASTExprVisitor& visitor, int depth) const override { 
        LOG_AST_VISIT(BinaryASTNode, depth);
        visitor.visit(*this, depth); 
    }

    TokenType type;
    ASTExprPtr left;
    ASTExprPtr right;
};

class UnaryASTNode : public ASTExprNode
{
public:
    UnaryASTNode(TokenType type,ASTExprPtr right) : type(type) {
        this->right = right;
        LOG_AST(UnaryASTNode);
    }
    virtual void accept(ASTExprVisitor& visitor, int depth) const override { 
        LOG_AST_VISIT(UnaryASTNode, depth);
        visitor.visit(*this, depth); 
    }

    TokenType type;
    ASTExprPtr right;
};

class CallASTNode : public ASTExprNode
{
public:
    CallASTNode(ASTExprPtr callee, Token paren, const std::vector<ASTExprPtr>& arguments) :
        callee(callee), paren(paren), arguments(arguments) {}

    virtual void accept(ASTExprVisitor& visitor, int depth) const override {
        LOG_AST_VISIT(CallASTNode, depth);
        visitor.visit(*this, depth);
    }

    ASTExprPtr callee;
    Token paren;
    std::vector<ASTExprPtr> arguments;
};

class LogicalASTNode : public ASTExprNode
{
    public:
	LogicalASTNode(TokenType type, ASTExprPtr left, ASTExprPtr right) : type(type) {
		this->left = left;
		this->right = right;
		LOG_AST(LogicalASTNode);
	}
	virtual void accept(ASTExprVisitor& visitor, int depth) const override { 
        LOG_AST_VISIT(LogicalASTNode, depth);
        visitor.visit(*this, depth); 
    }

	TokenType type;
	ASTExprPtr left;
	ASTExprPtr right;
};
