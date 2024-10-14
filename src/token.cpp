#include "token.h"

#include <fmt/core.h>

Token Tokenizer::peek()
{
    if (!_hasPeek) {
        bool savedDebug = debug;
        debug = false;
        _peek = get();
        debug = savedDebug;
        _hasPeek = true;
    }
    return _peek;
}


Token Tokenizer::get()
{
    if (_hasPeek) {
        _hasPeek = false;
        if (debug) fmt::print("{}\n", _peek.dump());
        return _peek;
    }

    skipWhitespace();
    if (_pos == _input.size()) {
		return Token(TokenType::eof);
	}

    const char c = _input[_pos];

    // Number:
    if (isDigitStart(c)) {
        const char* start = &_input[_pos];
        char* end = nullptr;
        double val = std::strtod(start, &end);
        if (end > start) {
            _pos += (end - start);
            Token tok(TokenType::NUMBER, std::string(start, end - start));
            tok.dValue = val;
            if (debug) fmt::print("{}\n", tok.dump());
            return tok;
        }
    }

    // Identifier or Keyword
    if (isAlpha(c)) {
        std::string t;
        t += c;
        _pos++;
        while (_pos < _input.size() && isAplhaNum(_input[_pos])) {
            t += _input[_pos];
            _pos++;
        }

        if (t == "var") return Token(TokenType::VAR);
        if (t == "return") return Token(TokenType::RET);

        Token token(TokenType::IDENT, t);
        if (debug) fmt::print("{}\n", token.dump());
        return token;
    }

    // Symbols
    std::string sym;
    sym += c;
    _pos++;
    Token token(TokenType::error);

    switch (c) {
    case '=': token = match('=') ? Token(TokenType::EQUAL_EQUAL, "==") : Token(TokenType::EQUAL, sym); break;

    case '+': token = Token(TokenType::PLUS, sym); break;
    case '-': token = Token(TokenType::MINUS, sym); break;
    case '*': token = Token(TokenType::MULT, sym); break;
    case '/': token = Token(TokenType::DIVIDE, sym); break;
    case '(': token = Token(TokenType::LEFT_PAREN, sym); break;
    case ')': token = Token(TokenType::RIGHT_PAREN, sym); break;
    case '{': token = Token(TokenType::LEFT_BRACE, sym); break;
    case '}': token = Token(TokenType::RIGHT_BRACE, sym); break;
    case '!': token = match('=') ? Token(TokenType::BANG_EQUAL, "!=") : Token(TokenType::BANG, sym); break;
    case '>': token = match('=') ? Token(TokenType::GREATER_EQUAL, ">=") : Token(TokenType::GREATER, sym); break;
    case '<': token = match('=') ? Token(TokenType::LESS_EQUAL, "<=") : Token(TokenType::LESS, sym); break;

    default:
        break;
    }
    if (debug) fmt::print("{}\n", token.dump());
    return token;
}

std::string Token::dump() const 
{
    static const char* name[static_cast<int>(TokenType::count)] = {
        "EOF",
        "ERROR",
        "NUMBER",
        "IDENT",

        "VAR",
        "RET",

        "PLUS",
        "MINUS",
        "MULT",
        "DIVIDE",
        "LEFT_PAREN",
        "RIGHT_PAREN",
        "LEFT_BRACE",
        "RIGHT_BRACE",
        "BANG",

        "EQUAL",
        "EQUAL_EQUAL",
        "BANG_EQUAL",
        "GREATER",
        "GREATER_EQUAL",
        "LESS",
        "LESS_EQUAL",
    };

    std::string r = name[static_cast<int>(type)];
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
