#include "token.h"
#include "errorreporting.h"

#include <fmt/core.h>
#include <assert.h>

Token Tokenizer::peek()
{
    if (!_hasPeek) {
        _peek = innerGet();
        _hasPeek = true;
    }
    return _peek;
}

void Tokenizer::skipWhitespace()
{
    while (_pos < _input.size()) {
        const char c = current();
		if (c == ' ' || c == '\t' || c == '\r') {
			_pos++;
		}
		else if (c == '\n') {
			_line++;
			_pos++;
		}
		else if (c == '/' && peekChar() == '/') {
			while (_pos < _input.size() && _input[_pos] != '\n') {
				_pos++;
			}
		}
		else {
			break;
		}
	}
}

Token Tokenizer::get()
{
    Token t = innerGet();
    if (debug)
        t.print();
    return t;
}


Token Tokenizer::innerGet()
{
    if (_hasPeek) {
        _hasPeek = false;
        return _peek;
    }

    skipWhitespace();
    if (_pos == _input.size()) {
		return Token(TokenType::eof, _line, "");
	}

    const char c = _input[_pos];

    // Number:
    if (isDigitStart(c)) {
        const char* start = &_input[_pos];
        char* end = nullptr;
        double val = std::strtod(start, &end);
        if (end > start) {
            _pos += (end - start);
            Token tok(TokenType::NUMBER, _line, std::string(start, end - start));
            tok.dValue = val;
            return tok;
        }
    }

    // String
    static const char SINGLE = '\'';
    static const char DOUBLE = '"';

    // FIXME: handle multi-line strings
    if (c == SINGLE || c == DOUBLE) {
		std::string t;
		_pos++;
		while (_pos < _input.size() && _input[_pos] != c) {
            char ch = _input[_pos];
            if (ch == '\r' || ch == '\n')
                break;

            t += ch;
			_pos++;
		}
        if (_input[_pos] != c) {
            ErrorReporter::report("fixme", _line, "Unterminated string");
			return Token(TokenType::error, _line, "");
        }

		_pos++;
		Token token(TokenType::STRING, _line, t);
		return token;
	}

    // Identifier or Keyword or Boolean
    if (isAlpha(c)) {
        std::string t;
        t += c;
        _pos++;
        while (_pos < _input.size() && isAplhaNum(_input[_pos])) {
            t += _input[_pos];
            _pos++;
        }

        if (t == "var") 
            return Token(TokenType::VAR, _line, t);
        if (t == "return") 
            return Token(TokenType::RETURN, _line, t);
        if (t == "print") 
            return Token(TokenType::PRINT, _line, t);
        if (t == "true") 
            return Token(TokenType::TRUE, _line, t);
        if (t == "false") 
            return Token(TokenType::FALSE, _line, t);
		if (t == "if")
			return Token(TokenType::IF, _line, t);
		if (t == "else")
			return Token(TokenType::ELSE, _line, t);
        if (t == "while")
            return Token(TokenType::WHILE, _line, t);

        Token token(TokenType::IDENT, _line, t);
        return token;
    }

    // Symbols
    std::string sym;
    sym += c;
    _pos++;
    Token token(TokenType::error, -1, "");

    switch (c) {
    case '=': token = match('=') ? Token(TokenType::EQUAL_EQUAL, _line, "==") : Token(TokenType::EQUAL, _line, sym); break;
    case '+': token = Token(TokenType::PLUS, _line, sym); break;
    case '-': token = Token(TokenType::MINUS, _line, sym); break;
    case '*': token = Token(TokenType::MULT, _line, sym); break;
    case '/': token = Token(TokenType::DIVIDE, _line, sym); break;
    case '(': token = Token(TokenType::LEFT_PAREN, _line, sym); break;
    case ')': token = Token(TokenType::RIGHT_PAREN, _line, sym); break;
    case '{': token = Token(TokenType::LEFT_BRACE, _line, sym); break;
    case '}': token = Token(TokenType::RIGHT_BRACE, _line, sym); break;
    case '[': token = Token(TokenType::LEFT_BRACKET, _line, sym); break;
    case ']': token = Token(TokenType::RIGHT_BRACKET, _line, sym); break;
    case '!': token = match('=') ? Token(TokenType::BANG_EQUAL, _line, "!=") : Token(TokenType::BANG, _line, sym); break;
    case '>': token = match('=') ? Token(TokenType::GREATER_EQUAL, _line, ">=") : Token(TokenType::GREATER, _line, sym); break;
    case '<': token = match('=') ? Token(TokenType::LESS_EQUAL, _line, "<=") : Token(TokenType::LESS, _line, sym); break;
    case ':': token = Token(TokenType::COLON, _line, sym); break;
    case ',': token = Token(TokenType::COMMA, _line, sym); break;
    case '&': 
        if(match('&')) return Token(TokenType::LOGIC_AND, _line, "&&"); 
        break;
    case '|': 
        if(match('|')) return Token(TokenType::LOGIC_OR, _line, "||"); 
        break;

    default:
        break;
    }
    if (token.type == TokenType::error) {
		ErrorReporter::report("fixme", _line, fmt::format("Unexpected character: {}", c));
	}
    return token;
}


std::string Token::toString(TokenType type)
{
    static const char* name[static_cast<int>(TokenType::count)] = {
        "EOF",
        "ERROR",

        "NUMBER",
        "STRING",
        "TRUE",
        "FALSE",
        "IDENT",

        "VAR",
        "RET",
        "PRINT",
		"IF",
		"ELSE",
        "WHILE",

        "PLUS",
        "MINUS",
        "MULT",
        "DIVIDE",

        "LEFT_PAREN",
        "RIGHT_PAREN",
        "LEFT_BRACE",
        "RIGHT_BRACE",
        "LEFT_BRACKET",
        "RIGHT_BRACKET",
        
        "BANG",
        "COLON",
        "COMMA",

        "EQUAL",
        "EQUAL_EQUAL",
        "BANG_EQUAL",
        "GREATER",
        "GREATER_EQUAL",
        "LESS",
        "LESS_EQUAL",

        "LOGIC_OR",
        "LOGIC_AND",
    };
    // Check the name and count list are in sync
    assert(name[static_cast<int>(TokenType::count) - 1] == std::string("LOGIC_AND"));
    return name[static_cast<int>(type)];
}

std::string Token::toString() const 
{
    std::string r = toString(type);

    if (type == TokenType::NUMBER) {
        r += ":";
        r += std::to_string(dValue);
    }
    else if (type == TokenType::IDENT) {
        r += ":";
        r += lexeme;
    }
    return r;
}

void Token::print() const
{
    fmt::print("[{:3}] {}\n", line, toString());
}
