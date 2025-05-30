#include "token.hpp"

#include <string_view>
#include <iostream>
#include <format>

Token::Token() : value{ "" }, general_type{ GeneralTokenType::OTHER },  token_type{ TokenType::NONE } {}

Token::Token(std::string_view value, GeneralTokenType general_type, TokenType token_type) : 
    value{ value }, general_type{ general_type }, token_type{ token_type } {}

void Token::print_token() const noexcept {
    std::cout << std::format("Value: {} | GTT: {} | TT: {}\n", 
        value, general_token_str.at(general_type), token_type_str.at(token_type));
}
