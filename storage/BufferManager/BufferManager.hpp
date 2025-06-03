#ifndef BUFFER_MANAGER_HPP
#define BUFFER_MANAGER_HPP

#include <memory>

#include "../../SchemaCatalog/SchemaCatalog/SchemaCatalog.hpp"
#include "../storage/page.hpp"
#include "../../ASTree/ASTree.hpp"

class BufferManager {
public:
    bool load_schema(const std::string&, SchemaCatalog&);
    void save_schema(const std::string&, const std::string&, const TableSchema&);
    void delete_schema(const std::string&, const std::string&, const std::string&, SchemaCatalog&);

    std::unique_ptr<TablePage> table_page_at(const std::string&, uint32_t);
    std::unique_ptr<TablePage> root_table_page(const std::string&);
    void write_page(const std::string&, const TablePage*);

    uint32_t new_page_id(const std::string&);
    void update_root_id(const std::string&, uint32_t);
    uint32_t get_root_id(const std::string&);

    Block data_to_block(const ASTree*, const ASTree*, const TableSchema&);

};

#endif