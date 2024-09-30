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
            return Token(TOK_NUMBER, number);
        }

        if (std::isalpha(_current)) {
            std::string identifier;
            while (std::isalnum(_current)) {
                identifier += _current;
                advance();
            }
            return Token(TOK_IDENTIFIER, identifier);
        }

        if (_current == '+') {
            advance();
            return Token(TOK_PLUS);
        }
        if (_current == '-') {
            advance();
            return Token(TOK_MINUS);
        }
        if (_current == '*') {
            advance();
            return Token(TOK_MUL);
        }
        if (_current == '/') {
            advance();
            return Token(TOK_DIV);
        }
        if (_current == '(') {
            advance();
            return Token(TOK_LPAREN);
        }
        if (_current == ')') {
            advance();
            return Token(TOK_RPAREN);
        }
        if (_current == '=') {
            advance();
            return Token(TOK_ASSIGN);
        }

        throw std::runtime_error("Invalid character");
    }

    return Token(TOK_EOF);
}
