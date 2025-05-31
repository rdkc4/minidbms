#include "BufferManager.hpp"

#include <array>
#include <cstdint>
#include <cstring>
#include <format>
#include <fstream>
#include <iostream>
#include <filesystem>

bool BufferManager::load_schema(const std::string& path, SchemaCatalog& schema_catalog) {
    std::ifstream file{ path, std::ios::binary };
    if(!file.is_open()){
        std::cerr << std::format("Unable to open '{}'", path);
        return false;
    }
    std::array<char, PAGE_SIZE> buffer;

    while(file.read(buffer.data(), PAGE_SIZE)) {
        uint32_t table_name_len, column_number;
        std::memcpy(&table_name_len, buffer.data(), sizeof(uint32_t));
        std::memcpy(&column_number, buffer.data() + sizeof(uint32_t), sizeof(uint32_t));
        size_t offset = 2 * sizeof(uint32_t);

        std::string table_name;
        table_name.resize(table_name_len);
        std::memcpy(&table_name[0], buffer.data() + offset, table_name_len);
        offset += table_name_len;

        std::vector<Column> columns;
        for(uint32_t i = 0; i < column_number; ++i){
            DataType type;
            std::memcpy(&type, buffer.data() + offset, sizeof(DataType));
            offset += sizeof(DataType);

            std::string column_name{buffer.data() + offset};
            columns.push_back(Column{ column_name, type});
            offset += 20;
        }
        TableSchema ts{table_name};
        for(const auto& col : columns){
            ts.add_column(col);
        }
        schema_catalog.add_table(ts);
    }
    return true;
}

void BufferManager::save_schema(const std::string& path, const TableSchema& ts) {
    SchemaPage page;
    page.table_name_len = static_cast<uint32_t>(ts.get_table_name().size());
    page.column_number = static_cast<uint32_t>(ts.columns_size());
    
    size_t offset = 0;
    std::memset(&page.data[0], 0, sizeof(page.data));
    
    std::memcpy(&page.data[offset], &ts.get_table_name()[0], ts.get_table_name().size());
    offset += ts.get_table_name().size();
    
    for(const auto& col : ts.get_columns()) {
        std::memcpy(&page.data[offset], &col.type, sizeof(uint8_t));
        std::memcpy(&page.data[offset + sizeof(col.type)], col.name.data(), col.name.size());
        offset += 20 + sizeof(col.type);
    }
    std::ofstream os{path, std::ios::binary | std::ios::app };
    if(!os.is_open()){
        std::cerr << std::format("Unable to open '{}'\n", path);
        return;
    }
    os.write(reinterpret_cast<const char*>(&page), sizeof(page));
}

void BufferManager::delete_schema(const std::string& path, const std::string& table_name, SchemaCatalog& schema_catalog) {
    std::fstream file{ path, std::ios::in | std::ios::out | std::ios::binary};
    if(!file.is_open()){
        std::cerr << std::format("Unable to open '{}'\n", path);
        return;
    }
    bool found = false;
    size_t current_page = 0;
    std::array<char, PAGE_SIZE> buffer;
    while(file.read(buffer.data(), PAGE_SIZE)){
        uint32_t table_name_len;
        std::memcpy(&table_name_len, buffer.data(), sizeof(uint32_t));
        std::string_view _table_name{ buffer.data() + 2 * sizeof(uint32_t), table_name_len};

        if(_table_name == table_name) {
            found = true;
            break;
        }
        ++current_page;
    }

    if(!found) return;

    std::streampos src_offset = file.tellg();
    std::streampos dst_offset = current_page * PAGE_SIZE;

    while (true) {
        file.seekg(src_offset);
        file.read(buffer.data(), PAGE_SIZE);
        if (file.gcount() == 0) break;

        file.seekp(dst_offset);
        file.write(buffer.data(), file.gcount());

        src_offset += PAGE_SIZE;
        dst_offset += PAGE_SIZE;
    }
    std::filesystem::resize_file(path, dst_offset);
    schema_catalog.drop_table(table_name);
}

Block BufferManager::data_to_block(const ASTree*, const TableSchema&) {
    Block b;
    return b;
}

void BufferManager::block_to_data(const Block block, const TableSchema&) {
    (void)block;
}
