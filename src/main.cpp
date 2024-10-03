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
            current_token = tokenizer.get_next_token();
        } else {
            throw std::runtime_error("Invalid syntax");
        }
    }

    ASTNode* factor() {
        Token token = current_token;
        if (token.type == TOK_NUMBER) {
            eat(TOK_NUMBER);
            return new NumberNode(std::stoi(token.value));
        } else if (token.type == TOK_IDENTIFIER) {
            std::string var_name = token.value;
            eat(TOK_IDENTIFIER);
            return new VariableNode(var_name);
        } else if (token.type == TOK_LPAREN) {
            eat(TOK_LPAREN);
            ASTNode* node = expr();
            eat(TOK_RPAREN);
            return node;
        }
        throw std::runtime_error("Invalid syntax");
    }

    ASTNode* term() {
        ASTNode* node = factor();
        while (current_token.type == TOK_MUL || current_token.type == TOK_DIV) {
            Token token = current_token;
            if (token.type == TOK_MUL) {
                eat(TOK_MUL);
                node = new BinaryOpNode('*', node, factor());
            } else if (token.type == TOK_DIV) {
                eat(TOK_DIV);
                node = new BinaryOpNode('/', node, factor());
            }
        }
        return node;
    }

    ASTNode* expr() {
        ASTNode* node = term();
        while (current_token.type == TOK_PLUS || current_token.type == TOK_MINUS) {
            Token token = current_token;
            if (token.type == TOK_PLUS) {
                eat(TOK_PLUS);
                node = new BinaryOpNode('+', node, term());
            } else if (token.type == TOK_MINUS) {
                eat(TOK_MINUS);
                node = new BinaryOpNode('-', node, term());
            }
        }
        return node;
    }

public:
    Parser(Tokenizer tokenizer) : tokenizer(tokenizer) {}

    ASTNode* parse() {
        current_token = tokenizer.get_next_token();
        if (current_token.type == TOK_IDENTIFIER) {
            Token var_token = current_token;
            eat(TOK_IDENTIFIER);
            eat(TOK_ASSIGN);
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
            delete root;
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
