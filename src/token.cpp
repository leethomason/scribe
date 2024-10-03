#include <stdexcept>

#include "token.h"

Token Tokenizer::getNext()
{
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
        // FIXME: check here for keyword

        return Token(TokenType::identifier, t);
    }

    // Symbols
    std::string sym;
    sym += c0;
    _pos++;

    switch (c0) {
    case '=': return Token(TokenType::assign, sym);
    case '+': return Token(TokenType::plus, sym);
    case '-': return Token(TokenType::minus, sym);
    case '*': return Token(TokenType::multiply, sym);
    case '/': return Token(TokenType::divide, sym);
    case '(': return Token(TokenType::leftParen, sym);
    case ')': return Token(TokenType::rightParen, sym);
    default:
        break;
    }

    return Token(TokenType::error);
}
