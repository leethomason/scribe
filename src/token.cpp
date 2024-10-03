#include <stdexcept>

#include "token.h"

size_t Tokenizer::readNumber(size_t start)
{
    if (isNumberStart(_input[start])) {
        start++;
        while(start < _input.size() && isNumber(_input[start])) {
			start++;
		}
    }
    return start;
}

Token Tokenizer::getNext(uint32_t classFlags)
{
    skipWhitespace();
    if (_pos == _input.size()) {
		return Token(TokenType::eof);
	}

    if (classFlags & cNumber) {
        size_t end = readNumber(_pos);
        if (end != _pos) {
            std::string number = _input.substr(_pos, end - _pos);
            _pos = end;
            return Token(TokenType::number, number);
        }
    }


    while (_pos < _input.size()) {


        if (std::isdigit(_current)) {
            std::string number;
            while (std::isdigit(_current)) {
                number += _current;
                advance();
            }
            return Token(TokenType::number, number);
        }

        if (std::isalpha(_current)) {
            std::string identifier;
            while (std::isalnum(_current)) {
                identifier += _current;
                advance();
            }
            return Token(TokenType::identifier, identifier);
        }

        if (_current == '+') {
            advance();
            return Token(TokenType::plus);
        }
        if (_current == '-') {
            advance();
            return Token(TokenType::minus);
        }
        if (_current == '*') {
            advance();
            return Token(TokenType::multiply);
        }
        if (_current == '/') {
            advance();
            return Token(TokenType::divide);
        }
        if (_current == '(') {
            advance();
            return Token(TokenType::leftParen);
        }
        if (_current == ')') {
            advance();
            return Token(TokenType::rightParen);
        }
        if (_current == '=') {
            advance();
            return Token(TokenType::assign);
        }

        throw std::runtime_error("Invalid character");
    }

    return Token(TokenType::eof);
}
