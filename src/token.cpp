#include "token.h"

#include <fmt/core.h>

Token Tokenizer::peekNext()
{
    if (!_hasPeek) {
        bool savedDebug = debug;
        debug = false;
        _peek = getNext();
        debug = savedDebug;
        _hasPeek = true;
    }
    return _peek;
}


Token Tokenizer::getNext()
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

    char c0 = _input[_pos];
    char c1 = (_pos + 1) < _input.size() ? _input[_pos + 1] : 0;
    char c2 = (_pos + 2) < _input.size() ? _input[_pos + 2] : 0;

    // Number:
    if (isNumberPart(c0)                                                // 0 .0
        || (isNumberStart(c0) && isDigit(c1))                           // -0
        || (isNumberStart(c0) && isNumberPart(c1) && isDigit(c2)))      // +.0
    {
        const char* start = &_input[_pos];
        char* end = nullptr;
        double val = std::strtod(start, &end);
        if (end > start) {
            _pos += (end - start);
            Token tok(TokenType::number, std::string(start, end - start));
            tok.dValue = val;
            if (debug) fmt::print("{}\n", tok.dump());
            return tok;
        }
    }

    // Identifier or Keyword
    if (isIdentStart(c0)) {
        std::string t;
        t += c0;
        _pos++;
        while (_pos < _input.size() && isIdent(_input[_pos])) {
            t += _input[_pos];
            _pos++;
        }

        if (t == "var") return Token(TokenType::var);
        if (t == "return") return Token(TokenType::ret);

        Token token(TokenType::identifier, t);
        if (debug) fmt::print("{}\n", token.dump());
        return token;
    }

    // Symbols
    std::string sym;
    sym += c0;
    _pos++;

    Token token(TokenType::error);

    switch (c0) {
    case '=': token = Token(TokenType::assign, sym); break;
    case '+': token = Token(TokenType::plus, sym); break;
    case '-': token = Token(TokenType::minus, sym); break;
    case '*': token = Token(TokenType::multiply, sym); break;
    case '/': token = Token(TokenType::divide, sym); break;
    case '(': token = Token(TokenType::leftParen, sym); break;
    case ')': token = Token(TokenType::rightParen, sym); break;
    default:
        break;
    }
    if (debug) fmt::print("{}\n", token.dump());
    return token;
}

std::string Token::dump() const 
{
    switch (type) {
    case TokenType::eof: return "EOF";
    case TokenType::error: return "ERROR";
    case TokenType::number: return "NUMBER: " + std::to_string(dValue);
    case TokenType::identifier: return "IDENTIFIER: " + value;
    case TokenType::var: return "VAR";
    case TokenType::ret: return "RET";
    case TokenType::assign: return "ASSIGN";
    case TokenType::plus: return "PLUS";
    case TokenType::minus: return "MINUS";
    case TokenType::multiply: return "MULTIPLY";
    case TokenType::divide: return "DIVIDE";
    case TokenType::leftParen: return "LEFT PAREN";
    case TokenType::rightParen: return "RIGHT PAREN";
    default: return "UNKNOWN";
    }
}
