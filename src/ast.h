#pragma once

#include "bytecode.h"
#include "value.h"
#include "token.h"

#include <map>
#include <string>
#include <vector>

// Abstract syntax tree (AST) nodes
class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual void evaluate(std::vector<Instruction>& bc, ConstPool& pool) = 0;
};

class ValueNode : public ASTNode {
    Value value;
public:
    ValueNode(const Value& value) : value(value) {}

    void evaluate(std::vector<Instruction>& bc, ConstPool& pool) override;
};

class DeclareVariableNode : public ASTNode {
    std::string name;
    ASTNode* expr = nullptr;
public:
    DeclareVariableNode(std::string name, ASTNode* expr) : name(name), expr(expr) {}
    ~DeclareVariableNode() { delete expr; }

    void evaluate(std::vector<Instruction>& bc, ConstPool& pool) override;
};

class BinaryOpNode : public ASTNode {
    TokenType type;
    ASTNode* left;
    ASTNode* right;
public:
    BinaryOpNode(TokenType type, ASTNode* left, ASTNode* right) : type(type), left(left), right(right) {}
    ~BinaryOpNode() { delete left; delete right; }
    void evaluate(std::vector<Instruction>& bc, ConstPool& pool) override;
};

class AssignmentNode : public ASTNode {
    std::string varName;
    ASTNode* expr;
public:
    AssignmentNode(std::string varName, ASTNode* expr) : varName(varName), expr(expr) {}
    ~AssignmentNode() { delete expr; }
    void evaluate(std::vector<Instruction>& bc, ConstPool& pool) override;
};
