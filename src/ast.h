#pragma once

#include "bytecode.h"
#include "value.h"
#include "token.h"

#include <map>
#include <string>
#include <vector>

class ASTNode;
class ValueASTNode;
class IdentifierASTNode;
class KeywordASTNode;
class BinaryASTNode;
class UnaryASTNode;

using ASTPtr = std::shared_ptr<ASTNode>;

class ASTVisitor
{
public:
    virtual void visit(const ValueASTNode&, int depth) = 0;   
    virtual void visit(const IdentifierASTNode&, int depth) = 0;
    virtual void visit(const KeywordASTNode&, int depth) = 0;
    virtual void visit(const BinaryASTNode&, int depth) = 0;
    virtual void visit(const UnaryASTNode&, int depth) = 0;
};

// Abstract syntax tree (AST) nodes
class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual void accept(ASTVisitor& visitor, int depth) const = 0;
};

class ValueASTNode : public ASTNode
{
public:
    ValueASTNode(const Value& value) : value(value) {}
    virtual void accept(ASTVisitor& visitor, int depth) const override { visitor.visit(*this, depth);}

    Value value;
};

class IdentifierASTNode : public ASTNode
{ 
public:
	IdentifierASTNode(const std::string& name) : name(name) {}
    virtual void accept(ASTVisitor& visitor, int depth) const override { visitor.visit(*this, depth); }

    std::string name;
};

class KeywordASTNode : public ASTNode
{
public:
    KeywordASTNode(TokenType token) : token(token) {}
    virtual void accept(ASTVisitor& visitor, int depth) const override { visitor.visit(*this, depth); }

    TokenType token = TokenType::error;
};

class BinaryASTNode : public ASTNode
{
public:
    BinaryASTNode(TokenType type, ASTPtr left, ASTPtr right) : type(type) {
        this->left = left;
        this->right = right;
    }
    virtual void accept(ASTVisitor& visitor, int depth) const override { 
        left->accept(visitor, depth + 1);
        right->accept(visitor, depth + 1);
        visitor.visit(*this, depth); 
    }

    TokenType type;
    ASTPtr left;
    ASTPtr right;
};

class UnaryASTNode : public ASTNode
{
public:
    UnaryASTNode(TokenType type,ASTPtr right) : type(type) {
        this->right = right;
    }
    virtual void accept(ASTVisitor& visitor, int depth) const override { 
        right->accept(visitor, depth + 1);
        visitor.visit(*this, depth); 
    }

    TokenType type;
    ASTPtr right;
};