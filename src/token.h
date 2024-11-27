#pragma once

#include <string>

enum class TokenType {
    eof,            // End of file/input
    error,          // Parsing error

    // Numbers & vars
    NUMBER,         // Number
    STRING,
    TRUE,
    FALSE,
    IDENT,          // Variable name

    // Keywords
    VAR,            // variable declaration
    RETURN,         // push to stack
    PRINT,	        // print to console
	IF,			    // if statement
	ELSE,           // else statement
    WHILE,		    // while statement

    // Sybols & operations
    PLUS,           // '+'
    MINUS,          // '-'
    MULT,           // '*'
    DIVIDE,         // '/'
    LEFT_PAREN,     // '('
    RIGHT_PAREN,    // ')'
    LEFT_BRACE,     // '{'
    RIGHT_BRACE,    // '}'
    LEFT_BRACKET,   // '['
    RIGHT_BRACKET,  // ']'
    BANG,           // '!'
    COLON,	        // ':'

    EQUAL,          // '='
    EQUAL_EQUAL,    // '=='
    BANG_EQUAL,     // '!='
    GREATER,        // '>'
    GREATER_EQUAL,  // '>='
    LESS,           // '<'
    LESS_EQUAL,     // '<='

    LOGIC_OR,       // '||'     // going with Swift here
    LOGIC_AND,      // '&&'

    count
};

// Token structure
struct Token {
    TokenType type;
    int line = 0;
    std::string lexeme;

    double dValue = 0;      // if number

    Token() : type(TokenType::error) {}
    Token(TokenType type, int line, std::string lexeme) : type(type), line(line), lexeme(lexeme) {}
    Token(double d, int line) : type(TokenType::NUMBER), line(line), dValue(d) {}

    bool isBinOp() const {
        return type >= TokenType::PLUS && type <= TokenType::DIVIDE;
    }

    static std::string toString(TokenType type);
    
    std::string toString() const;
    void print() const;
};

// Tokenizer (scanner or lexar) to break input into tokens
class Tokenizer {
public:
    Tokenizer(const std::string& input) : _input(input) {}
    Token get();
    Token peek();

    bool done() const { return _pos >= _input.size(); }

    static void test();

    bool debug = false;

private:
    Token innerGet();

    const std::string& _input;
    size_t _pos = 0;
    Token _peek;
    bool _hasPeek = false;
    int _line = 0;

    char current() const {
        return _input[_pos];
    }

    char peekChar() const {
		return _pos + 1 < _input.size() ? _input[_pos + 1] : 0;
	}

    void advance() {
        if (_pos > _input.size())
            _pos++;
    }

    bool inputEnd() const {
		return _pos >= _input.size();
	}

    bool match(char m) {
        if (_pos >= _input.size()) return false;
        if (_input[_pos] == m) {
            _pos++;
            return true;
        }
        return false;
    }

    void skipWhitespace();

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
