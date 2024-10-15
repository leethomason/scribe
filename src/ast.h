#pragma once

#include "bytecode.h"
#include "value.h"
#include "token.h"

#include <map>
#include <string>
#include <vector>

class ASTNode;
using ASTPtr = std::shared_ptr<ASTNode>;

class ValueASTNode;
class IdentifierASTNode;
class BinaryASTNode;

class ASTVisitor
{
public:
    virtual void visit(const ValueASTNode&, int depth) = 0;   
    virtual void visit(const IdentifierASTNode&, int depth) = 0;
    virtual void visit(const BinaryASTNode&, int depth) = 0;
};

// Abstract syntax tree (AST) nodes
class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual void accept(ASTVisitor& visitor, int depth) const = 0;

    virtual void evaluate(std::vector<Instruction>& bc, ConstPool& pool) = 0;
};

class ValueASTNode : public ASTNode
{
public:
    ValueASTNode(const Value& value) : value(value) {}
    virtual void accept(ASTVisitor& visitor, int depth) const override { visitor.visit(*this, depth);}

    void evaluate(std::vector<Instruction>& bc, ConstPool& pool) override;
  
    Value value;
};

class IdentifierASTNode : public ASTNode
{ 
public:
	IdentifierASTNode(const std::string& name) : name(name) {}
    virtual void accept(ASTVisitor& visitor, int depth) const override { visitor.visit(*this, depth); }

    void evaluate(std::vector<Instruction>& bc, ConstPool& pool) override;

    std::string name;
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

    void evaluate(std::vector<Instruction>& bc, ConstPool& pool) override;

    TokenType type;
    ASTPtr left;
    ASTPtr right;
};