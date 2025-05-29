#ifndef PARSER_HPP
#define PARSER_HPP

#include <memory>

#include "../lexer/lexer.hpp"
#include "../ASTree/ASTree.hpp"

class Parser {
public:
    Parser(Lexer&);

    std::unique_ptr<ASTree> parse_script();

private:
    Lexer& lexer;
    Token token;
    size_t token_idx;

    const Token& token_at(size_t) const noexcept;
    const Token& next_token() noexcept;
    const Token& peek() const noexcept;

    void consume_token(TokenType);

    std::unique_ptr<ASTree> parse_query();
    std::unique_ptr<ASTree> parse_select();
    std::unique_ptr<ASTree> parse_create();
    std::unique_ptr<ASTree> parse_insert();
    std::unique_ptr<ASTree> parse_update();
    std::unique_ptr<ASTree> parse_delete();
    std::unique_ptr<ASTree> parse_drop();

    std::unique_ptr<ASTree> parse_select_columns();
    std::unique_ptr<ASTree> parse_columns();
    std::unique_ptr<ASTree> parse_condition();
    std::unique_ptr<ASTree> parse_orderby();
    std::unique_ptr<ASTree> parse_table_columns();
    std::unique_ptr<ASTree> parse_values();
    std::unique_ptr<ASTree> parse_assignments();
    std::unique_ptr<ASTree> parse_id();
    std::unique_ptr<ASTree> parse_value();

};

#endif