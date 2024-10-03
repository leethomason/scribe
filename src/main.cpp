#include "token.h"
#include "ast.h"
#include "machine.h"

#include <iostream>
#include <string>
#include <map>
#include <cctype>
#include <stdexcept>
#include <sstream>


// Parser to convert tokens to AST
class Parser {
    Tokenizer tokenizer;
    Token current_token;

    void eat(TokenType type) {
        if (current_token.type == type) {
            current_token = tokenizer.getNext();
        } else {
            throw std::runtime_error("Invalid syntax");
        }
    }

    ASTNode* factor() {
        Token token = current_token;
        if (token.type == TokenType::number) {
            eat(TokenType::number);
            return new NumberNode(std::stoi(token.value));
        } else if (token.type == TokenType::identifier) {
            std::string var_name = token.value;
            eat(TokenType::identifier);
            return new VariableNode(var_name);
        } else if (token.type == TokenType::leftParen) {
            eat(TokenType::leftParen);
            ASTNode* node = expr();
            eat(TokenType::rightParen);
            return node;
        }
        throw std::runtime_error("Invalid syntax");
    }

    ASTNode* term() {
        ASTNode* node = factor();
        while (current_token.type == TokenType::multiply || current_token.type == TokenType::divide) {
            Token token = current_token;
            if (token.type == TokenType::multiply) {
                eat(TokenType::multiply);
                node = new BinaryOpNode('*', node, factor());
            } else if (token.type == TokenType::divide) {
                eat(TokenType::divide);
                node = new BinaryOpNode('/', node, factor());
            }
        }
        return node;
    }

    ASTNode* expr() {
        ASTNode* node = term();
        while (current_token.type == TokenType::plus || current_token.type == TokenType::minus) {
            Token token = current_token;
            if (token.type == TokenType::plus) {
                eat(TokenType::plus);
                node = new BinaryOpNode('+', node, term());
            } else if (token.type == TokenType::minus) {
                eat(TokenType::minus);
                node = new BinaryOpNode('-', node, term());
            }
        }
        return node;
    }

public:
    Parser(Tokenizer tokenizer) : tokenizer(tokenizer) {}

    ASTNode* parse() {
        current_token = tokenizer.getNext();
        if (current_token.type == TokenType::identifier) {
            Token var_token = current_token;
            eat(TokenType::identifier);
            eat(TokenType::assign);
            ASTNode* expr_node = expr();
            return new AssignmentNode(var_token.value, expr_node);
        } else {
            return expr();
        }
    }
};

// Interpreter
class Interpreter {
    std::map<std::string, int> variables;

public:
    void interpret(std::string input) {
        try {
            Tokenizer tokenizer(input);
            Parser parser(tokenizer);
            ASTNode* root = parser.parse();
            int result = root->evaluate(variables);
            std::cout << "Result: " << result << std::endl;
        } catch (std::exception& ex) {
            std::cout << "Error: " << ex.what() << std::endl;
        }
    }
};

int main() 
{
	Machine::test();

    Interpreter interpreter;
    std::string line;

    while (true) {
        std::cout << ">> ";
        std::getline(std::cin, line);
        if (line == "exit") break;
        interpreter.interpret(line);
    }
    return 0;
}
