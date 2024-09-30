#pragma once

#include <string>

// Token types
enum TokenType {
    TOK_EOF,      // End of file/input
    TOK_NUMBER,   // Number
    TOK_IDENTIFIER, // Variable name
    TOK_ASSIGN,   // '='
    TOK_PLUS,     // '+'
    TOK_MINUS,    // '-'
    TOK_MUL,      // '*'
    TOK_DIV,      // '/'
    TOK_LPAREN,   // '('
    TOK_RPAREN,   // ')'
};

// Token structure
struct Token {
    TokenType type;
    std::string value;

    Token() : type(TOK_EOF) {}
    Token(TokenType type, std::string value = "") : type(type), value(value) {}
};

// Tokenizer to break input into tokens
class Tokenizer {
public:
    Tokenizer(std::string input) : _input(input), _pos(0), _current(input[0]) {}
    Token get_next_token();

private:
    std::string _input;
    size_t _pos = 0;
    char _current = 0;

    void advance() {
        _pos++;
        _current = (_pos < _input.length()) ? _input[_pos] : 0;
    }

    void skip_whitespace() {
        while (std::isspace(_current)) {
            advance();
        }
    }
};
