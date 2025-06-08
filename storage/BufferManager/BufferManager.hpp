#ifndef BUFFER_MANAGER_HPP
#define BUFFER_MANAGER_HPP

#include <memory>
#include <variant>

#include "../../SchemaCatalog/SchemaCatalog/SchemaCatalog.hpp"
#include "../storage/page.hpp"
#include "../../ASTree/ASTree.hpp"

class BufferManager {
public:
    bool load_schema(const std::string&, SchemaCatalog&) const;
    void save_schema(const std::string&, const std::string&, const TableSchema&) const;
    void delete_schema(const std::string&, const std::string&, const std::string&, SchemaCatalog&) const;

    std::unique_ptr<TablePage> table_page_at(const std::string&, uint32_t) const;
    std::unique_ptr<TablePage> root_table_page(const std::string&) const;
    void write_page(const std::string&, const TablePage*) const;

    uint32_t new_page_id(const std::string&) const;
    void update_root_id(const std::string&, uint32_t) const;
    uint32_t get_root_id(const std::string&) const;

    Block data_to_block(const ASTree*, const ASTree*, const TableSchema&) const;
    std::unordered_map<std::string, std::variant<std::string, uint32_t>> block_to_data(const Block&, const TableSchema&) const;
    void delete_table_data(const std::string& table_path) const;

    void init_table(const std::string& table_path) const;

};

#endif