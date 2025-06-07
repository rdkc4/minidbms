#ifndef PAGE_HPP
#define PAGE_HPP

#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <string>

constexpr size_t PAGE_SIZE_ = 4096;

constexpr size_t MAX_KEY_SIZE = 21; // including '\0'
constexpr size_t BLOCK_SIZE_ = 512;

constexpr size_t MAX_TABLE_LEN = 51;
constexpr size_t MAX_COLUMN_LEN = 21; // including '\0'
constexpr size_t MAX_STRING_LEN = 21; // including '\0'

constexpr size_t T = 4;

#pragma pack(push, 1) // 4096B
struct SchemaPage { 
    uint32_t table_name_len;
    uint32_t column_number;
    char data[PAGE_SIZE_ - sizeof(uint32_t) * 2];

    SchemaPage() : table_name_len{ 0 }, column_number{ 0 } {
        std::memset(data, 0, sizeof(data));
    }
};
#pragma pack(pop)

#pragma pack(push, 1) //512B
struct Block {
    uint8_t key_type;
    uint8_t is_deleted;
    char key[MAX_KEY_SIZE];
    char value[BLOCK_SIZE_ - MAX_KEY_SIZE - sizeof(key_type) - sizeof(is_deleted)];

    Block() : key_type{ 0 }, is_deleted{ 0 } {
        std::memset(key, 0, sizeof(key));
        std::memset(value, 0, sizeof(value));
    }
    Block(std::string c) : key_type{ 0 }, is_deleted{ 0 } { 
        std::memcpy(&key, c.data(), c.length());
        std::memset(value, 0, sizeof(value));
    }
};
#pragma pack(pop)

#pragma pack(push, 1) // 4096B
struct TablePage {
    uint8_t n;
    uint8_t is_leaf;
    Block blocks[2 * T - 1]; //3584 -> T = 4
    uint32_t children[2 * T]; //32 -> T = 4
    uint32_t page_id;
    char alignment[PAGE_SIZE_ - sizeof(children) - sizeof(blocks) - sizeof(n) - sizeof(is_leaf) - sizeof(page_id)];

    TablePage() : n{ 0 }, is_leaf{ 1 }, page_id{ 0 } {
        std::memset(blocks, 0, sizeof(blocks));
        std::memset(children, 0, sizeof(children));
        std::memset(alignment, 0, sizeof(alignment));
    }
    TablePage(uint8_t is_leaf) : n{ 0 }, is_leaf{ is_leaf }, page_id{ 0 } {
        std::memset(blocks, 0, sizeof(blocks));
        std::memset(children, 0, sizeof(children));
        std::memset(alignment, 0, sizeof(alignment));
    }
    TablePage(uint8_t n, uint8_t is_leaf) : n{ n }, is_leaf{ is_leaf }, page_id{ 0 } {
        std::memset(blocks, 0, sizeof(blocks));
        std::memset(children, 0, sizeof(children));
        std::memset(alignment, 0, sizeof(alignment));
    }
};
#pragma pack(pop)

#endif