#include "SchemaCatalog.hpp"
#include <optional>

void SchemaCatalog::add_table(const TableSchema& table){
    tables.emplace(table.get_table_name(), table);
}

std::optional<std::reference_wrapper<const TableSchema>> SchemaCatalog::get_table(const std::string& table_name) const noexcept {
    auto it = tables.find(table_name);
    if(it != tables.end()){
        return std::cref(it->second);
    }
    return std::nullopt;
}