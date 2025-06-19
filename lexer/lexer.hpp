#ifndef LEXER_HPP
#define LEXER_HPP

#include <string_view>
#include <string>
#include <vector>
#include "../token/token.hpp"

class Lexer {
public:
    explicit Lexer(std::string_view);
    
    bool completedTokenization() const noexcept;
    void tokenize();

    const Token& token_at(size_t) const noexcept;

private:
    std::string script;
    size_t position;
    std::vector<Token> tokens;

    void updatePosition() noexcept;
    void updatePosition(size_t n) noexcept;

    void get_id();
    void get_number();
    void get_delimiter(char);
    void get_string();

    bool is_keyword(const std::string&) const noexcept;
    bool is_type(const std::string& type) const noexcept;
    bool is_operator(const std::string& op) const noexcept;
    bool is_delimiter(char) const noexcept;

    void print_tokens() const noexcept;

};

#endif