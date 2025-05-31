#include "TableSchema.hpp"
#include <optional>
#include <iostream>

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

size_t TableSchema::columns_size() const noexcept {
    return columns.size();
}

const std::vector<Column>& TableSchema::get_columns() const noexcept {
    return columns;
}

std::optional<std::reference_wrapper<const Column>> TableSchema::get_column(const std::string& col_name) const noexcept {
    for(const auto& col : columns){
        if(col.name == col_name){
            return std::cref(col);
        }
    }
    return std::nullopt;
}

bool TableSchema::column_exists(const std::string& col_name) const noexcept {
    for(const auto& col : columns){
        if(col.name == col_name){
            return true;
        }
    }
    return false;
}

void TableSchema::print_column_names() const {
    for(const auto& col : columns){
        std::cout << std::format("Column name: {}, type: {}\n", col.name, data_type_str.at(col.type));
    }
}