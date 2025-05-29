#ifndef ASTREE_HPP
#define ASTREE_HPP

#include <memory>
#include <vector>

#include "defs/astdefs.hpp"
#include "../token/token.hpp"

class ASTree {
public:
    ASTree(Token, ASTNodeType);

    void add_child(std::unique_ptr<ASTree>&&);

    const Token& get_token() const noexcept;
    ASTNodeType get_type() const noexcept;
    const ASTree* child_at(size_t) const noexcept;
    const std::vector<std::unique_ptr<ASTree>>& get_children() const noexcept;

    std::string ast_str() const;
    void traverse(size_t) const;

private:
    Token token;
    ASTNodeType node_type;
    std::vector<std::unique_ptr<ASTree>> children;


};

#endif