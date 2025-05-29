#include "TableSchema.hpp"
#include <optional>

TableSchema::TableSchema(std::string_view name) : table_name{ name } {} 

void TableSchema::add_column(const Column& col){
    for(const auto& column : columns){
        if(col.name == column.name) return;
    }
    columns.push_back(col);
}

const std::string& TableSchema::get_table_name() const noexcept {
    return table_name;
}

std::optional<std::reference_wrapper<const Column>> TableSchema::get_column(const std::string& col_name) const noexcept {
    for(const auto& col : columns){
        if(col.name == col_name){
            return std::cref(col);
        }
    }
    return std::nullopt;
}