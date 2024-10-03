#include <stdexcept>

#include "token.h"

Token Tokenizer::get_next_token() {
    while (_current) {
        if (std::isspace(_current)) {
            skip_whitespace();
            continue;
        }

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
