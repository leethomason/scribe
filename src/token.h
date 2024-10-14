#pragma once

#include <string>

enum class TokenType {
    eof,        // End of file/input
    error,      // Parsing error

    // Numbers & vars
    NUMBER,     // Number
    IDENT, // Variable name

    // Keywords
    VAR,        // variable declaration
    RET,        // push to stack

    // Sybols & operations
    PLUS,       // '+'
    MINUS,      // '-'
    MULT,   // '*'
    DIVIDE,     // '/'
    LEFT_PAREN,  // '('
    RIGHT_PAREN, // ')'
    LEFT_BRACE,  // '{'
    RIGHT_BRACE, // '}'
    BANG,       // '!'

    EQUAL,     // '='
    EQUAL_EQUAL, // '=='
    BANG_EQUAL,  // '!='
    GREATER,     // '>'
    GREATER_EQUAL, // '>='
    LESS,        // '<'
    LESS_EQUAL,  // '<='

    count
};

// Token structure
struct Token {
    TokenType type;
    std::string lexeme;
    double dValue = 0;

    Token() : type(TokenType::eof) {}
    Token(TokenType type, std::string lexeme = "") : type(type), lexeme(lexeme) {}
    Token(double d) : type(TokenType::NUMBER), dValue(d) {}

    bool isBinOp() const {
        return type >= TokenType::PLUS && type <= TokenType::DIVIDE;
    }

    std::string dump() const;
};

// Tokenizer (scanner or lexar) to break input into tokens
class Tokenizer {
public:
    Tokenizer(const std::string& input) : _input(input) {}
    Token get();
    Token peek();

    static void test();

    bool debug = false;

private:
    const std::string& _input;
    size_t _pos = 0;
    Token _peek;
    bool _hasPeek = false;

    void advance() {
        _pos++;
    }

    bool match(char m) {
        if (_pos >= _input.size()) return false;
        if (_input[_pos] == m) {
            _pos++;
            return true;
        }
        return false;
    }

    void skipWhitespace() {
        while (std::isspace(_input[_pos])) {
            advance();
        }
    }

    static bool isDigit(char c) {
		return c >= '0' && c <= '9';
	}
    static bool isDigitStart(char c) {
		return isDigit(c) || c == '.';
	}
    static bool isAlpha(char c) {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
    }
    static bool isAplhaNum(char c) {
        return isDigit(c) || isAlpha(c);
    }
};
