#ifndef BUFFER_MANAGER_HPP
#define BUFFER_MANAGER_HPP

#include "../../ASTree/ASTree.hpp"
#include "../../SchemaCatalog/SchemaCatalog/SchemaCatalog.hpp"
#include "../storage/page.hpp"

class BufferManager {
public:
    bool load_schema(const std::string&, SchemaCatalog&);
    void save_schema(const std::string&, const std::string&, const TableSchema&);
    void delete_schema(const std::string&, const std::string&, const std::string&, SchemaCatalog&);

    Block data_to_block(const ASTree*, const TableSchema&);
    void block_to_data(const Block block, const TableSchema&);


};

#endif