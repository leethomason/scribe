#pragma once

#include "ast.h"

class Interpreter : public ASTStmtVisitor 
{
public:
    Value interpret(std::string input);

    virtual void visit(const ASTExprStmtNode&) override;
    virtual void visit(const ASTPrintStmtNode&) override;
};

