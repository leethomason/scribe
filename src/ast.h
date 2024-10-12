#pragma once

#include "bytecode.h"
#include "value.h"
#include "token.h"

#include <map>
#include <string>
#include <vector>

class ASTNode;
using ASTPtr = std::shared_ptr<ASTNode>;

// Abstract syntax tree (AST) nodes
class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual void evaluate(std::vector<Instruction>& bc, ConstPool& pool) = 0;
    virtual void dump(int depth) const = 0;

    ASTPtr left;
    ASTPtr right;
};

class ValueASTNode : public ASTNode
{
public:
    ValueASTNode(const Value& value) : value(value) {}
    void evaluate(std::vector<Instruction>& bc, ConstPool& pool) override;
    virtual void dump(int depth) const override;
  
    Value value;
};

class IdentifierASTNode : public ASTNode
{ 
public:
	IdentifierASTNode(const std::string& name) : name(name) {}
    void evaluate(std::vector<Instruction>& bc, ConstPool& pool) override;
    virtual void dump(int depth) const override;

    std::string name;
};

class BinaryASTNode : public ASTNode
{
public:
    BinaryASTNode(TokenType type, ASTPtr left, ASTPtr right) : type(type) {
        this->left = left;
        this->right = right;
    }
    void evaluate(std::vector<Instruction>& bc, ConstPool& pool) override;
    virtual void dump(int depth) const override;

    TokenType type;
};