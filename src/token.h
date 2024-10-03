#pragma once

#include <string>

// Token types
enum class TokenType {
    eof,        // End of file/input
    number,     // Number
    identifier, // Variable name
    assign,     // '='
    plus,       // '+'
    minus,      // '-'
    multiply,   // '*'
    divide,     // '/'
    leftParen,  // '('
    rightParen, // ')'
};

// Token structure
struct Token {
    TokenType type;
    std::string value;

    Token() : type(TokenType::eof) {}
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
