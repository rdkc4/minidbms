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

const std::unordered_map<TokenType, std::string> token_type_str {
    {TokenType::SELECT, "SELECT"},
    {TokenType::FROM, "FROM"},
    {TokenType::WHERE, "WHERE"},
    {TokenType::INSERT, "INSERT"},
    {TokenType::INTO, "INTO"},
    {TokenType::VALUES, "VALUES"},
    {TokenType::ORDER, "ORDER"},
    {TokenType::BY, "BY"},
    {TokenType::LIMIT, "LIMIT"},
    {TokenType::UPDATE, "UPDATE"},
    {TokenType::SET, "SET"},
    {TokenType::DELETE, "DELETE"},
    {TokenType::CREATE, "CREATE"},
    {TokenType::DROP, "DROP"},
    {TokenType::TABLE, "TABLE"},
    {TokenType::_NULL, "NULL"},
    {TokenType::AND, "AND"},
    {TokenType::OR, "OR"},
    {TokenType::ID, "ID"},
    {TokenType::STRING_LITERAL, "STRING_LITERAL"},
    {TokenType::NUMBER_LITERAL, "NUMBER_LITERAL"},
    {TokenType::_NULL, "NULL"},
    {TokenType::EQUAL, "EQUAL"},
    {TokenType::GREATER, "GREATER"},
    {TokenType::GREATER_EQUAL, "GREATER_EQUAL"},
    {TokenType::LESS, "LESS"},
    {TokenType::LESS_EQUAL, "LESS_EQUAL"},
    {TokenType::NOT_EQUAL, "NOT_EQUAL"},
    {TokenType::COMMA, "COMMA"},
    {TokenType::LPAREN, "LPAREN"},
    {TokenType::RPAREN, "RPAREN"},
    {TokenType::SEMICOLON, "SEMICOLON"},
    {TokenType::APOSTROPHE, "APOSTROPHE"},
    {TokenType::ASTERISK, "ASTERISK"},
    {TokenType::NUMBER, "NUMBER"},
    {TokenType::VARCHAR, "VARCHAR"},
    {TokenType::NONE, "NONE"},
    {TokenType::END, "END"}
};

const std::unordered_map<GeneralTokenType, std::string> general_token_str {
    {GeneralTokenType::KEYWORD, "KEYWORD"},
    {GeneralTokenType::TYPE, "TYPE"},
    {GeneralTokenType::OPERATOR, "OPERATOR"},
    {GeneralTokenType::DELIMITER, "DELIMITER"},
    {GeneralTokenType::LITERAL, "LITERAL"},
    {GeneralTokenType::OTHER, "OTHER"}
};
