#ifndef BTREE_HPP
#define BTREE_HPP 

#include <cstdlib>
#include <cstring>
#include <memory>

#include "../storage/page.hpp"
#include "../BufferManager/BufferManager.hpp"

class BTree {
private:
    void split(TablePage*, int, TablePage*, const std::string&, BufferManager&);

    void insert_nonfull(std::unique_ptr<TablePage>&&, Block&, const std::string&, BufferManager&);

    std::unique_ptr<Block> search(std::unique_ptr<TablePage>, char*, const std::string&, BufferManager&);

    void traverse(const std::string&, uint32_t, BufferManager&, const TableSchema&, int);

    void del_blocks(const std::string&, BufferManager&, const TableSchema&, const ASTree*);

public:
    void insert(Block&, BufferManager&, const std::string&);

    std::unique_ptr<Block> search(char*, const std::string&, BufferManager&);

    void traverse(const std::string&, BufferManager&, const TableSchema&);

    void del(const std::string&, BufferManager&, const TableSchema&, const ASTree*);
};

#endif
