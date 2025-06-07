#include "BufferManager.hpp"

#include <array>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <format>
#include <fstream>
#include <ios>
#include <iosfwd>
#include <iostream>
#include <filesystem>
#include <stdexcept>
#include <unordered_map>
#include <variant>
#include <vector>

#ifdef _WIN32
    #include <winsock2.h>
#else
    #include <arpa/inet.h>
#endif

bool BufferManager::load_schema(const std::string& path, SchemaCatalog& schema_catalog) {
    std::ifstream file{ path, std::ios::binary };
    if(!file.is_open()){
        std::cerr << std::format("Unable to open '{}'", path);
        return false;
    }
    std::array<char, PAGE_SIZE_> buffer;

    while(file.read(buffer.data(), PAGE_SIZE_)) {
        uint32_t table_name_len, column_number;
        std::memcpy(&table_name_len, buffer.data(), sizeof(uint32_t));
        std::memcpy(&column_number, buffer.data() + sizeof(uint32_t), sizeof(uint32_t));
        table_name_len = ntohl(table_name_len);
        column_number = ntohl(column_number);
        size_t offset = 2 * sizeof(uint32_t);

        std::string table_name;
        table_name.resize(table_name_len);
        std::memcpy(&table_name[0], buffer.data() + offset, table_name_len);
        offset += table_name_len;

        std::vector<Column> columns;
        for(uint32_t i = 0; i < column_number; ++i){
            DataType type;
            std::memcpy(&type, buffer.data() + offset, sizeof(DataType));
            uint8_t is_key;
            std::memcpy(&is_key, buffer.data() + offset + sizeof(DataType), sizeof(uint8_t));

            std::string column_name{buffer.data() + offset + sizeof(DataType) + sizeof(uint8_t), MAX_COLUMN_LEN};
            column_name = column_name.c_str();
            columns.push_back(Column{ column_name, type, static_cast<bool>(is_key) });
            offset += MAX_COLUMN_LEN + sizeof(DataType) + sizeof(uint8_t);
        }
        TableSchema table_schema{table_name};
        for(const auto& col : columns){
            table_schema.add_column(col);
        }
        schema_catalog.add_table(table_schema);
    }
    return true;
}

void BufferManager::save_schema(const std::string& schema_path, const std::string& table_path, const TableSchema& table_schema) {
    SchemaPage schema_page;
    schema_page.table_name_len = htonl(static_cast<uint32_t>(table_schema.get_table_name().size()));
    schema_page.column_number = htonl(static_cast<uint32_t>(table_schema.columns_size()));
    
    size_t offset = 0;
    
    std::memcpy(&schema_page.data[offset], &table_schema.get_table_name()[0], table_schema.get_table_name().size());
    offset += table_schema.get_table_name().size();
    
    for(const auto& col : table_schema.get_columns()) {
        std::memcpy(&schema_page.data[offset], &col.type, sizeof(uint8_t));
        uint8_t is_key = static_cast<uint8_t>(col.is_key);
        std::memcpy(&schema_page.data[offset + sizeof(uint8_t)], &is_key, sizeof(uint8_t));
        std::memcpy(&schema_page.data[offset + sizeof(col.type) + sizeof(is_key)], col.name.data(), col.name.size());
        offset += MAX_COLUMN_LEN + sizeof(col.type) + sizeof(is_key);
    }
    {
        std::ofstream os{schema_path, std::ios::binary | std::ios::app };
        if(!os.is_open()){
            std::cerr << std::format("Unable to open '{}'\n", schema_path);
            return;
        }
        os.write(reinterpret_cast<const char*>(&schema_page), sizeof(schema_page));
    }
    std::ofstream os{table_path + table_schema.get_table_name() + ".db", std::ios::binary};
    TablePage table_page;
    uint32_t initial_root_id = 0;
    os.write(reinterpret_cast<const char*>(&initial_root_id), sizeof(initial_root_id)); //offset of a root on first 4 bytes
    os.write(reinterpret_cast<const char*>(&table_page), sizeof(table_page));
}

// should optimize, instead of shifting, just swap with the last one
void BufferManager::delete_schema(const std::string& schema_path, const std::string& table_path, const std::string& table_name, SchemaCatalog& schema_catalog) {
    std::fstream file{ schema_path, std::ios::in | std::ios::out | std::ios::binary};
    if(!file.is_open()){
        std::cerr << std::format("Unable to open '{}'\n", schema_path);
        return;
    }
    bool found = false;
    size_t current_page = 0;
    std::array<char, PAGE_SIZE_> buffer;
    while(file.read(buffer.data(), PAGE_SIZE_)){
        uint32_t table_name_len;
        std::memcpy(&table_name_len, buffer.data(), sizeof(uint32_t));
        table_name_len = ntohl(table_name_len);
        std::string_view _table_name{ buffer.data() + 2 * sizeof(uint32_t), table_name_len};

        if(_table_name == table_name) {
            found = true;
            break;
        }
        ++current_page;
    }

    if(!found) return;

    std::streampos src_offset = file.tellg();
    std::streampos dst_offset = current_page * PAGE_SIZE_;

    while (true) {
        file.seekg(src_offset);
        file.read(buffer.data(), PAGE_SIZE_);
        if (file.gcount() == 0) break;

        file.seekp(dst_offset);
        file.write(buffer.data(), PAGE_SIZE_);

        src_offset += PAGE_SIZE_;
        dst_offset += PAGE_SIZE_;
    }
    std::filesystem::resize_file(schema_path, dst_offset);
    schema_catalog.drop_table(table_name);
    std::filesystem::remove(table_path + table_name + ".db");
}

std::unique_ptr<TablePage> BufferManager::table_page_at(const std::string& table_path, uint32_t page_id){
    std::ifstream file{ table_path, std::ios::binary };
    if(!file.is_open()) {
        return nullptr;
    }

    file.seekg(static_cast<std::streampos>(sizeof(uint32_t) + page_id * PAGE_SIZE_));
    if(!file || file.eof()) return nullptr;

    std::array<char, PAGE_SIZE_> buffer;
    file.read(buffer.data(), PAGE_SIZE_);
    
    if(file.gcount() != PAGE_SIZE_) return nullptr;

    std::unique_ptr<TablePage> table_page = std::make_unique<TablePage>();
    std::memcpy(table_page.get(), buffer.data(), PAGE_SIZE_);
    
    for(uint8_t i = 0; i < table_page->n + 1; ++i){
        table_page->children[i] = ntohl(table_page->children[i]);
    }
    table_page->page_id = ntohl(table_page->page_id);

    return table_page;
}

std::unique_ptr<TablePage> BufferManager::root_table_page(const std::string& table_path){
    std::ifstream file{ table_path, std::ios::binary };
    if(!file.is_open()) {
        return nullptr;
    }
    std::array<char, PAGE_SIZE_> buffer;
    file.read(buffer.data(), sizeof(uint32_t));
    if(file.gcount() != sizeof(uint32_t)) return nullptr;

    uint32_t root_id{};
    std::memcpy(&root_id, buffer.data(), sizeof(uint32_t));
    root_id = ntohl(root_id);

    file.seekg(static_cast<std::streampos>(PAGE_SIZE_ * root_id + sizeof(uint32_t)));
    file.read(buffer.data(), PAGE_SIZE_);
    if(file.gcount() != PAGE_SIZE_) return nullptr;

    TablePage table_page;
    std::memcpy(&table_page, buffer.data(), PAGE_SIZE_);

    for(uint8_t i = 0; i < table_page.n + 1; ++i){
        table_page.children[i] = ntohl(table_page.children[i]);
    }
    table_page.page_id = ntohl(table_page.page_id);

    return std::make_unique<TablePage>(table_page);
}

void BufferManager::write_page(const std::string& table_path, const TablePage* table_page){
    std::fstream file{ table_path, std::ios::binary | std::ios::in | std::ios::out};
    if(!file.is_open()){
        std::cerr << std::format("Unable to open '{}'\n", table_path);
        return;
    }
    TablePage page{ *table_page };
    for(uint8_t i = 0; i < page.n + 1; ++i){
        page.children[i] = htonl(page.children[i]);
    }
    page.page_id = htonl(page.page_id);
    
    file.seekp(static_cast<std::streampos>(table_page->page_id * PAGE_SIZE_ + sizeof(uint32_t)));
    file.write(reinterpret_cast<const char*>(&page), PAGE_SIZE_);    
}

uint32_t BufferManager::new_page_id(const std::string& table_path) {
    std::ifstream file{ table_path, std::ios::binary | std::ios::ate };
    if(!file.is_open()){
        throw std::runtime_error(std::format("Unable to open '{}'\n", table_path));
    }
    
    std::streampos file_size = file.tellg();
    if(file_size <= static_cast<std::streampos>(sizeof(uint32_t))){
        throw std::runtime_error(std::format("Corrupted table '{}'\n", table_path));
    }
    return (static_cast<uint32_t>(file_size) - sizeof(uint32_t)) / PAGE_SIZE_;
}

void BufferManager::update_root_id(const std::string& table_path, uint32_t root_id) {
    std::fstream file{ table_path, std::ios::binary | std::ios::in | std::ios::out };
    file.seekp(std::ios::beg);

    root_id = htonl(root_id);
    file.write(reinterpret_cast<const char*>(&root_id), sizeof(root_id));
}

uint32_t BufferManager::get_root_id(const std::string& table_path) {
    std::ifstream file{ table_path, std::ios::binary };
    if(!file.is_open()){
        throw std::runtime_error(std::format("Unable to open '{}'", table_path));
    }
    uint32_t root_id{};
    file.read(reinterpret_cast<char*>(&root_id), sizeof(root_id));
    return ntohl(root_id);
}

Block BufferManager::data_to_block(const ASTree* columns, const ASTree* values, const TableSchema& table_schema) {
    std::unordered_map<std::string, std::string> col_map;
    Block block;

    size_t offset{ 0 };
    for(size_t i = 0; i < columns->children_size(); ++i){
        col_map[columns->child_at(i)->get_token().value] = values->child_at(i)->get_token().value;
    }
    const size_t columns_size{ table_schema.columns_size() };
    for(size_t i = 0; i < columns_size; ++i){
        const Column& column{ table_schema.get_column_at(i) };
        auto col_it = col_map.find(column.name);
        if(col_it != col_map.end()){
            if(column.is_key){
                block.key_type = static_cast<uint8_t>(column.type);
                if(column.type == DataType::NUMBER){
                    uint32_t val = htonl(std::stoul(col_it->second));
                    std::memcpy(block.key, &val, sizeof(val));
                }
                else{
                    std::memcpy(block.key, col_it->second.c_str(), col_it->second.length());
                }
            }
            else{
                uint8_t type = static_cast<uint8_t>(column.type);
                std::memcpy(block.value + offset, &type, sizeof(type));
                if(column.type == DataType::NUMBER){
                    uint32_t val = htonl(std::stoul(col_it->second));
                    std::memcpy(block.value + offset + sizeof(type), &val, sizeof(val));
                }
                else{
                    std::memcpy(block.value + offset + sizeof(type), col_it->second.c_str(), col_it->second.length());
                }
            }
            offset += (column.is_key ?  0 : (sizeof(DataType) + (column.type == DataType::VARCHAR ? MAX_STRING_LEN : sizeof(uint32_t))));
        }   
    }
    return block;
}

std::unordered_map<std::string, std::variant<std::string, uint32_t>> BufferManager::block_to_data(const Block& block, const TableSchema& table_schema) {
    std::unordered_map<std::string, std::variant<std::string, uint32_t>> data;
    const size_t columns_size{ table_schema.columns_size() };
    size_t offset{ 0 };
    for(size_t i = 0; i < columns_size; ++i){
        const Column& column{ table_schema.get_column_at(i) };
        if(column.is_key){
            if(static_cast<DataType>(block.key_type) == DataType::VARCHAR){
                data[column.name] = std::string{ block.key, MAX_COLUMN_LEN }.c_str();
            }
            else{
                uint32_t val{};
                std::memcpy(&val, block.key, sizeof(val));
                data[column.name] = ntohl(val);
            }
        }
        else{
            DataType type;
            std::memcpy(&type, block.value + offset, sizeof(DataType));
            if(type == DataType::VARCHAR){
                data[column.name] = std::string { block.value + offset + sizeof(DataType), MAX_COLUMN_LEN }.c_str();
            }
            else{
                uint32_t val{};
                std::memcpy(&val, block.value + offset + sizeof(DataType), sizeof(val));
                data[column.name] = ntohl(val);
            }
            offset += sizeof(DataType) + (type == DataType::VARCHAR ? MAX_STRING_LEN : sizeof(uint32_t));
        }
    }
    return data;
}
