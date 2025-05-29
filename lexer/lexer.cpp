#include "lexer.hpp"
#include "defs/lexdefs.hpp"
#include <cctype>
#include <format>
#include <stdexcept>
#include <string_view>

Lexer::Lexer(std::string_view query) : query{ query }, position{ 0 } {}

void Lexer::tokenize() {
    const size_t query_size{ query.size() };
    while(position < query_size){
        char curr = query[position];
        
        if(std::isspace(curr)){
            updatePosition();
        }
        else if(std::isalpha(curr)){
            get_id();
        }
        else if(std::isdigit(curr)){
            get_number();
        }
        else if(is_delimiter(curr)){
            get_delimiter(curr);
        }
        else if(is_operator(std::string{&query[position], 2})){
            std::string op{ &query[position], 2 };
            tokens.push_back(Token{op, GeneralTokenType::OPERATOR, operators.at(op)});
            updatePosition(2);
        }
        else if(is_operator(std::string{query[position]})){
            std::string op{ &query[position], 1 };
            tokens.push_back(Token{op, GeneralTokenType::OPERATOR, operators.at(op)});
            updatePosition();
        }
        else if(curr == '\''){
            get_string();
        }
        else if(curr == '*'){
            tokens.push_back(Token{std::string_view{&query[position], 1}, GeneralTokenType::OTHER, TokenType::ASTERISK});
            updatePosition();
        }
        else{
            throw std::runtime_error(std::format("Invalid token: {}\n", curr));
        }
    }
    tokens.push_back(Token{"", GeneralTokenType::OTHER, TokenType::END});
    print_tokens();
}

const Token& Lexer::token_at(size_t n) const noexcept {
    return n < tokens.size() ? tokens[n] : tokens.back();
}

void Lexer::updatePosition() noexcept {
    ++position;
}

void Lexer::updatePosition(size_t n) noexcept {
    position += n;
}

void Lexer::get_id(){
    size_t start{ position };
    while(std::isalnum(query[position])){
        updatePosition();
    }
    std::string id{&query[start], position-start};
    if(is_keyword(id)){
        tokens.push_back(Token{id, GeneralTokenType::KEYWORD, keywords.at(id)});
    }
    else if(is_type(id)){
        tokens.push_back(Token{id, GeneralTokenType::TYPE, types.at(id)});
    }
    else{
        tokens.push_back(Token{id, GeneralTokenType::OTHER, TokenType::ID});
    }
}

void Lexer::get_number(){
    size_t start{ position };
    while(std::isdigit(query[position])){
        updatePosition();
    }
    tokens.push_back(Token{std::string_view{&query[start], position - start}, GeneralTokenType::LITERAL, TokenType::NUMBER_LITERAL});
}

void Lexer::get_delimiter(char curr){
    tokens.push_back(Token{std::string_view{&curr, 1}, GeneralTokenType::DELIMITER, delimiters.at(curr)});
    updatePosition();
}

void Lexer::get_string(){
    updatePosition();
    size_t start{ position };
    while(query[position] != '\0' && query[position] != '\''){
        updatePosition();
    }
    if(query[position] == '\0'){
        throw std::runtime_error(std::format("Invalid string literal"));
    }
    tokens.push_back(Token{std::string_view{&query[start], position - start}, GeneralTokenType::LITERAL, TokenType::STRING_LITERAL});
    updatePosition();
}

bool Lexer::is_keyword(const std::string& id) const noexcept {
    return keywords.find(id) != keywords.end();
}

bool Lexer::is_type(const std::string& type) const noexcept {
    return types.find(type) != types.end();
}

bool Lexer::is_operator(const std::string& op) const noexcept {
    return operators.find(op) != operators.end();
}

bool Lexer::is_delimiter(char del) const noexcept {
    return delimiters.find(del) != delimiters.end();
}

void Lexer::print_tokens() const noexcept {
    for(const auto& token : tokens){
        token.print_token();
    }   
}