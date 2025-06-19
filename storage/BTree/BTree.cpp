#include "BTree.hpp"

#include <cstring>
#include <iostream>
#include <string>

void BTree::split(TablePage* x, int i, TablePage* y, const std::string& table_path, BufferManager& buffer_manager) {
    std::unique_ptr<TablePage> z = std::make_unique<TablePage>(TablePage{ y->is_leaf });
    z->n = T - 1;
    z->page_id = buffer_manager.new_page_id(table_path);

    for(int j = 0; j < static_cast<int>(T) - 1; ++j) {
        std::memcpy(&z->blocks[j], &y->blocks[T + j], sizeof(Block));
    }
    if(y->is_leaf == 0) {
        for(int j = 0; j < static_cast<int>(T); ++j) {
            z->children[j] = y->children[T + j];
        }
    }
    y->n = T - 1;

    for(int j = x->n; j >= i + 1; --j) {
        x->children[j + 1] = x->children[j];
    }
    x->children[i + 1] = z->page_id;

    for(int j = x->n - 1; j >= i; --j){
        std::memcpy(&x->blocks[j + 1], &x->blocks[j], sizeof(Block));
    }
    std::memcpy(&x->blocks[i], &y->blocks[T - 1], sizeof(Block));
    ++x->n;

    buffer_manager.write_page(table_path, x);
    buffer_manager.write_page(table_path, y);
    buffer_manager.write_page(table_path, z.get());
}

void BTree::insert_nonfull(std::unique_ptr<TablePage>&& page, Block& block, const std::string& table_path, BufferManager& buffer_manager){
    int i = page->n - 1;

    if(page->is_leaf == 1) {
        while(i >= 0 && std::strcmp(block.key, page->blocks[i].key) < 0) {
            std::memcpy(&page->blocks[i+1], &page->blocks[i], sizeof(Block));
            --i;
        }
        std::memcpy(&page->blocks[i + 1], &block, sizeof(Block));
        ++page->n;
        buffer_manager.write_page(table_path, page.get());
    }
    else {
        while(i >= 0 && std::strcmp(block.key, page->blocks[i].key) < 0) {
            --i;
        }
        ++i;
        std::unique_ptr<TablePage> page_i = buffer_manager.table_page_at(table_path, page->children[i]);

        if(page_i->n == 2 * T - 1) {
            split(page.get(), i, page_i.get(), table_path, buffer_manager);
            if(std::strcmp(block.key,page->blocks[i].key) > 0) {
                ++i;
                page_i = buffer_manager.table_page_at(table_path, page->children[i]);
            }
        }
        insert_nonfull(std::move(page_i), block, table_path, buffer_manager);
    }
}

std::unique_ptr<Block> BTree::search(std::unique_ptr<TablePage> page, char* key, const std::string& table_path, BufferManager& buffer_manager) {
    uint32_t i = 0;
    while(i < page->n && std::strcmp(key, page->blocks[i].key) > 0) {
        ++i;
    }
    if(i < page->n && std::strcmp(key, page->blocks[i].key) == 0){
        return std::make_unique<Block>(page->blocks[i]);
    }
    else if(page->is_leaf) {
        return nullptr;
    }
    std::unique_ptr<TablePage> page_i = buffer_manager.table_page_at(table_path, page->children[i]);
    return search(std::move(page_i), key, table_path, buffer_manager);
}

void BTree::insert(Block& block, BufferManager& buffer_manager, const std::string& table_path) {
    std::unique_ptr<TablePage> root = buffer_manager.root_table_page(table_path);
    if(root == nullptr) return;

    if(root->n == 2 * T - 1){
        std::unique_ptr<TablePage> s = std::make_unique<TablePage>(TablePage{ 0 });
        s->page_id = buffer_manager.new_page_id(table_path);

        if(s->page_id == 0){
            return;
        }

        s->children[0] = root->page_id;
        buffer_manager.update_root_id(table_path, s->page_id);
        buffer_manager.write_page(table_path, s.get());

        split(s.get(), 0, root.get(), table_path, buffer_manager);
        insert_nonfull(std::move(s), block, table_path, buffer_manager);
    }
    else{
        insert_nonfull(std::move(root), block, table_path, buffer_manager);
    }
}

std::unique_ptr<Block> BTree::search(char* key, const std::string& table_path, BufferManager& buffer_manager) {
    std::unique_ptr<TablePage> root_page = buffer_manager.root_table_page(table_path);
    return search(std::move(root_page), key, table_path, buffer_manager);
}

void BTree::traverse(const std::string& table_path, uint32_t page_id, BufferManager& buffer_manager, const TableSchema& table_schema, int padding){
    auto page = buffer_manager.table_page_at(table_path, page_id);
    std::cout << std::format("{}Page ID: {}, n: {}\n", std::string(padding * 4, ' '), page->page_id, page->n);
    for(uint8_t i = 0; i < page->n; ++i){
        std::cout << std::format("{}Key: {}\n", std::string(padding * 4, ' '), page->blocks[i].key);
        auto data = buffer_manager.block_to_data(page->blocks[i], table_schema);
        for(const Column& col : table_schema.get_columns()){
            std::visit([&](const auto& val) {
                std::cout << std::format("{}{}: {}\n", std::string(padding * 4, ' '), col.name, val);
            }, data[col.name]);
        }
    }
    if(page->is_leaf == 0){
        for(uint8_t i = 0; i < page->n + 1; ++i){
            traverse(table_path, page->children[i], buffer_manager, table_schema, padding + 1);
        }
    }
}

void BTree::traverse(const std::string& table_path, BufferManager& buffer_manager, const TableSchema& table_schema){
    traverse(table_path, buffer_manager.get_root_id(table_path), buffer_manager, table_schema, 0);
}

void BTree::del(const std::string& table_path, BufferManager& buffer_manager, const TableSchema& table_schema, const ASTree* _delete){
    if(_delete->children_size() == 1){
        buffer_manager.delete_all_data(table_path);
    }
    else{
        del_blocks(table_path, buffer_manager, table_schema, _delete->child_at(1));
    }
}

//TODO deletion with condition
void BTree::del_blocks(const std::string&, BufferManager&, const TableSchema&, const ASTree*){

}

// conditions aren't implemented at the moment
void BTree::select(const std::string& table_path, BufferManager& buffer_manager, const TableSchema& table_schema, const ASTree* _select){
    if(_select->children_size() < 3 || (_select->children_size() == 3 && _select->child_at(3)->get_type() != ASTNodeType::CONDITION)){
        uint32_t root_id = buffer_manager.get_root_id(table_path);
        select_no_condition(table_path, root_id, buffer_manager, table_schema, _select);
    }
}

void BTree::select_no_condition(const std::string& table_path, uint32_t page_id, BufferManager& buffer_manager, const TableSchema& table_schema, const ASTree* _select){
    auto page = buffer_manager.table_page_at(table_path, page_id);
    for(uint32_t i = 0; i < page->n; ++i){
        if(!page->is_leaf){
            select_no_condition(table_path, page->children[i], buffer_manager, table_schema, _select);
        }
        
        if(page->blocks[i].is_deleted) continue;

        std::string line;
        auto block_data = buffer_manager.block_to_data(page->blocks[i], table_schema);
        bool select_all = _select->child_at(0)->child_at(0)->get_token().token_type == TokenType::ASTERISK;

        if(select_all){
            for(const auto& col : table_schema.get_columns()){
                std::visit([&](const auto& val) {
                    line += std::format("{}: {}|", col.name, val);
                }, block_data[col.name]);
            }
            std::cout << line << '\n';
        }
        else{
            for(const auto& col : _select->child_at(0)->get_children()){
                std::visit([&](const auto& val) {
                    line += std::format("{}: {}|", col->get_token().value, val);
                }, block_data[col->get_token().value]);
            }
            std::cout << line << '\n';
        }
    }
    if(!page->is_leaf){
        select_no_condition(table_path, page->children[page->n], buffer_manager, table_schema, _select);
    }
}