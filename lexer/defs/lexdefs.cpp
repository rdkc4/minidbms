#include "lexdefs.hpp"
#include <string>

const std::unordered_map<std::string, TokenType> keywords { 
    {"SELECT", TokenType::SELECT},
    {"FROM", TokenType::FROM},
    {"WHERE", TokenType::WHERE},
    {"INSERT", TokenType::INSERT},
    {"INTO", TokenType::INTO},
    {"VALUES", TokenType::VALUES},
    {"ORDER", TokenType::ORDER},
    {"BY", TokenType::BY},
    {"LIMIT", TokenType::LIMIT},
    {"UPDATE", TokenType::UPDATE},
    {"SET", TokenType::SET},
    {"DELETE", TokenType::DELETE},
    {"CREATE", TokenType::CREATE},
    {"DROP", TokenType::DROP},
    {"TABLE", TokenType::TABLE},
    {"NULL", TokenType::_NULL},
    {"AND", TokenType::AND},
    {"OR", TokenType::OR},
    {"PRIMARY", TokenType::PRIMARY},
    {"KEY", TokenType::KEY},
    {"NULL", TokenType::_NULL}
};

const std::unordered_map<std::string, TokenType> operators {
    {"=", TokenType::EQUAL},
    {">", TokenType::GREATER},
    {">=", TokenType::GREATER_EQUAL},
    {"<", TokenType::LESS},
    {"<=", TokenType::LESS_EQUAL},
    {"<>", TokenType::NOT_EQUAL},
    {"!=", TokenType::NOT_EQUAL}
};

const std::unordered_map<char, TokenType> delimiters {
    {',', TokenType::COMMA},
    {'(', TokenType::LPAREN},
    {')', TokenType::RPAREN},
    {';', TokenType::SEMICOLON}
};

const std::unordered_map<std::string, TokenType> types {
    {"VARCHAR", TokenType::VARCHAR},
    {"NUMBER", TokenType::NUMBER}
};