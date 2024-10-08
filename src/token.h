#pragma once

#include <string>

enum class TokenType {
    eof,        // End of file/input
    error,      // Parsing error

    // Numbers & vars
    number,     // Number
    identifier, // Variable name

    // Keywords
    var,        // variable declaration
    ret,        // push to stack

    // Sybols & operations
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
    double dValue = 0;

    Token() : type(TokenType::eof) {}
    Token(TokenType type, std::string value = "") : type(type), value(value) {}
    Token(double d) : type(TokenType::number), dValue(d) {}
};

// Tokenizer to break input into tokens
class Tokenizer {
public:
    Tokenizer(const std::string& input) : _input(input) {}
    Token getNext();
    Token peekNext();

    static void test();

private:
    const std::string& _input;
    size_t _pos = 0;
    Token _peek;
    bool _hasPeek = false;

    void advance() {
        _pos++;
    }

    void skipWhitespace() {
        while (std::isspace(_input[_pos])) {
            advance();
        }
    }

    static bool isDigit(char c) {
		return c >= '0' && c <= '9';
	}
    static bool isNumberPart(char c) {
        return isDigit(c) || c == '.';
	}
    static bool isNumberStart(char c) {
        return isNumberPart(c) || c == '-' || c == '+';
    }
    static bool isIdent(char c) {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_' || (c >= '0' && c <= '9');
    }
    static bool isIdentStart(char c) {
		return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
	}
};
