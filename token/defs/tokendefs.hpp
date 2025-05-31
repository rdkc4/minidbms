#ifndef TOKENDEFS_HPP
#define TOKENDEFS_HPP

#include <unordered_map>
#include <string>

enum class TokenType { SELECT, FROM, WHERE, INSERT, INTO, VALUES, AND, OR, ID, STRING_LITERAL, NUMBER_LITERAL, 
    EQUAL, GREATER, GREATER_EQUAL, LESS, LESS_EQUAL, NOT_EQUAL, COMMA, LPAREN, RPAREN, SEMICOLON, APOSTROPHE, 
    ORDER, BY, LIMIT, UPDATE, SET, DELETE, CREATE, DROP, TABLE, _NULL, ASTERISK, END, VARCHAR, NUMBER, PRIMARY, KEY, NONE };

extern const std::unordered_map<TokenType, std::string> token_type_str;

enum class GeneralTokenType { KEYWORD, TYPE, OPERATOR, DELIMITER, LITERAL, OTHER };

extern const std::unordered_map<GeneralTokenType, std::string> general_token_str;

#endif