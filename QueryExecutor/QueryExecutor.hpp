#ifndef QUERY_EXECUTOR_HPP
#define QUERY_EXECUTOR_HPP

#include "../ASTree/ASTree.hpp"
#include "../SchemaCatalog/SchemaCatalog/SchemaCatalog.hpp"
#include "../storage/BufferManager/BufferManager.hpp"
#include "../storage/BTree/BTree.hpp"
#include <filesystem>

class QueryExecutor {
public:
    QueryExecutor(SchemaCatalog&, BufferManager&, BTree&);

    void execute_script(const ASTree*);

private:
    SchemaCatalog& schema_catalog;
    BufferManager buffer_manager;
    BTree btree;

    const std::filesystem::path METADATA_PATH{ "metadata" };
    const std::filesystem::path SCHEMA_PATH =  METADATA_PATH / "schema" / "schema.db";
    const std::filesystem::path TABLES_PATH = METADATA_PATH / "tables/";

    void execute_query(const ASTree*);
    void execute_select(const ASTree*);
    void execute_create(const ASTree*);
    void execute_insert(const ASTree*);
    void execute_update(const ASTree*);
    void execute_delete(const ASTree*);
    void execute_drop(const ASTree*);

};

#endif