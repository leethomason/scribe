#pragma once

#include <map>
#include <string>
#include <stdexcept>

// Abstract syntax tree (AST) nodes
class ASTNode {
public:
    virtual int evaluate(std::map<std::string, int>& variables) = 0;
};

class NumberNode : public ASTNode {
    int value;
public:
    NumberNode(int value) : value(value) {}
    int evaluate(std::map<std::string, int>&) override {
        return value;
    }
};

class VariableNode : public ASTNode {
    std::string name;
public:
    VariableNode(std::string name) : name(name) {}
    int evaluate(std::map<std::string, int>& variables) override {
        return variables[name];
    }
};

class BinaryOpNode : public ASTNode {
    char op;
    ASTNode* left;
    ASTNode* right;
public:
    BinaryOpNode(char op, ASTNode* left, ASTNode* right) : op(op), left(left), right(right) {}
    int evaluate(std::map<std::string, int>& variables) override {
        switch (op) {
        case '+': return left->evaluate(variables) + right->evaluate(variables);
        case '-': return left->evaluate(variables) - right->evaluate(variables);
        case '*': return left->evaluate(variables) * right->evaluate(variables);
        case '/': return left->evaluate(variables) / right->evaluate(variables);
        default: throw std::runtime_error("Invalid operator");
        }
    }
};

class AssignmentNode : public ASTNode {
    std::string variable_name;
    ASTNode* expr;
public:
    AssignmentNode(std::string variable_name, ASTNode* expr) : variable_name(variable_name), expr(expr) {}
    int evaluate(std::map<std::string, int>& variables) override {
        int result = expr->evaluate(variables);
        variables[variable_name] = result;
        return result;
    }
};
