#include "ASTree.hpp"
#include "defs/astdefs.hpp"

#include <iostream>
#include <format>

ASTree::ASTree(Token token, ASTNodeType type) : token{ token }, node_type{ type } {} 

void ASTree::add_child(std::unique_ptr<ASTree>&& child) {
    children.push_back(std::move(child));
}

const Token& ASTree::get_token() const noexcept {
    return token;
}

ASTNodeType ASTree::get_type() const noexcept {
    return node_type;
}

size_t ASTree::children_size() const noexcept {
    return children.size();
}

const ASTree* ASTree::child_at(size_t n) const noexcept {
    return n < children.size() ? children[n].get() : nullptr;
}

const std::vector<std::unique_ptr<ASTree>>& ASTree::get_children() const noexcept {
    return children;
}

std::string ASTree::ast_str() const {
    return std::format("AST: {}| Token value: {}", ast_node_str.at(node_type),  token.value);
}

void ASTree::traverse(size_t offset) const {
    std::cout << std::format("{}|-> {}\n", std::string(offset*2, ' '), ast_str());
    for(const auto& child:children){
        child->traverse(offset+1);
    }
}