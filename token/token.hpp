#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>
#include "defs/tokendefs.hpp"

struct Token {
    std::string value;
    GeneralTokenType general_type;
    TokenType token_type;

    Token();
    Token(std::string_view);
    Token(std::string_view, GeneralTokenType, TokenType);
    void print_token() const noexcept;
};

#endif