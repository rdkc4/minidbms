#include "TableSchema.hpp"
#include <optional>
#include <iostream>
#include <stdexcept>

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

const Column& TableSchema::get_column_at(size_t n) const noexcept {
    return columns[n];
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

const Column& TableSchema::get_key_column() const {
    for(const auto& col : columns){
        if(col.is_key){
            return col;
        }
    }
    throw std::runtime_error("None of the columns is key\n");
}

size_t TableSchema::get_column_index(const std::string& col_name) const {
    for(size_t i = 0; i < columns.size(); ++i){
        if(columns[i].name == col_name){
            return i;
        }
    }
    throw std::runtime_error(std::format("Column '{}' not found\n", col_name));
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
        std::cout << std::format("Column name: {}, type: {}, key: {}\n", col.name, data_type_str.at(col.type), col.is_key ? "True" : "False");
    }
}