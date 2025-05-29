#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>
#include <unordered_map>

enum class TokenType { SELECT, FROM, WHERE, INSERT, INTO, VALUES, AND, OR, ID, STRING_LITERAL, NUMBER_LITERAL, 
    EQUAL, GREATER, GREATER_EQUAL, LESS, LESS_EQUAL, NOT_EQUAL, COMMA, LPAREN, RPAREN, SEMICOLON, APOSTROPHE, 
    ORDER, BY, LIMIT, UPDATE, SET, DELETE, CREATE, DROP, TABLE, _NULL, ASTERISK, END, VARCHAR, NUMBER, NONE };

extern const std::unordered_map<TokenType, std::string> token_type_str;

enum class GeneralTokenType { KEYWORD, TYPE, OPERATOR, DELIMITER, LITERAL, OTHER };

extern const std::unordered_map<GeneralTokenType, std::string> general_token_str;

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