#ifndef LEXDEFS_HPP
#define LEXDEFS_HPP

#include <string>
#include <unordered_map>
#include "../../token/token.hpp"

extern const std::unordered_map<std::string, TokenType> keywords;

extern const std::unordered_map<std::string, TokenType> operators;

extern const std::unordered_map<char, TokenType> delimiters;

#endif