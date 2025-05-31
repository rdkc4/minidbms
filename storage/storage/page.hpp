#ifndef PAGE_HPP
#define PAGE_HPP

#include <cstdlib>
#include <cstdint>

constexpr size_t PAGE_SIZE = 4096;

constexpr size_t MAX_KEY_SIZE = 31;
constexpr size_t MAX_VALUE_SIZE = 478;

constexpr size_t T = 4;

#pragma pack(push, 1) // 4096b
struct SchemaPage { 
    uint32_t table_name_len;
    uint32_t column_number;
    char data[PAGE_SIZE - sizeof(uint32_t) * 2];
};
#pragma pack(pop)

#pragma pack(push, 1) //512b
struct Block {
    uint8_t key_len;
    char key[MAX_KEY_SIZE];
    uint16_t value_len;
    char value[MAX_VALUE_SIZE];
};
#pragma pack(pop)

#pragma pack(push, 1) // 4096b
struct Page {
    uint8_t n;
    uint8_t is_leaf;
    Block blocks[2 * T - 1]; //3584 -> T = 4
    uint16_t offset[2 * T]; //16 -> T = 4
    char alignment[PAGE_SIZE - sizeof(offset) - sizeof(blocks) - sizeof(n) - sizeof(is_leaf)];
};
#pragma pack(pop)

#endif