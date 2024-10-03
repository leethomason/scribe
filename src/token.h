#pragma once

#include <string>

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
    enum {
        cNumber = 0x01,
        cIdent = 0x02,
        cOperator = 0x04,
    };

    Tokenizer(const std::string& input) : _input(input) {}
    Token getNext(uint32_t classFlags);

private:
    const std::string& _input;
    size_t _pos = 0;

    void advance() {
        _pos++;
    }

    void skipWhitespace() {
        while (std::isspace(_pos)) {
            advance();
        }
    }

    static bool isDigit(char c) {
		return c >= '0' && c <= '9';
	}
    static bool isNumber(char c) {
        return isDigit(c) || c == '.';
	}
    static bool isNumberStart(char c) {
        return isNumber(c) || c == '-' || c == '+';
    }
    static bool isIdent(char c) {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_' || (c >= '0' && c <= '9');
    }
    static bool isIdentStart(char c) {
		return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
	}

    size_t readIdentifier(size_t start);
    size_t readNumber(size_t start);
};
