#ifndef PAGE_HPP
#define PAGE_HPP

#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <string>

constexpr size_t PAGE_SIZE = 4096;

constexpr size_t MAX_KEY_SIZE = 31;
constexpr size_t MAX_VALUE_SIZE = 480;

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
    uint8_t key_type;
    char key[MAX_KEY_SIZE];
    char value[MAX_VALUE_SIZE];

    Block() {}
    Block(std::string c){ std::memcpy(&key, c.data(), c.length());} //temporary
};
#pragma pack(pop)

#pragma pack(push, 1) // 4096b
struct TablePage {
    uint8_t n;
    uint8_t is_leaf;
    Block blocks[2 * T - 1]; //3584 -> T = 4
    uint32_t children[2 * T]; //32 -> T = 4
    uint32_t page_id;
    char alignment[PAGE_SIZE - sizeof(children) - sizeof(blocks) - sizeof(n) - sizeof(is_leaf) - sizeof(page_id)];

    TablePage() : n{ 0 }, is_leaf{ 1 }, page_id{ 0 } {}
    TablePage(uint8_t is_leaf) : n{ 0 }, is_leaf{ is_leaf }, page_id{ 0 } {}
    TablePage(uint8_t n, uint8_t is_leaf) : n{ n }, is_leaf{ is_leaf }, page_id{ 0 } {}
};
#pragma pack(pop)

#endif