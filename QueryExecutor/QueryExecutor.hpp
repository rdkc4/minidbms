#ifndef QUERY_EXECUTOR_HPP
#define QUERY_EXECUTOR_HPP

#include "../ASTree/ASTree.hpp"
#include "../SchemaCatalog/SchemaCatalog/SchemaCatalog.hpp"

class QueryExecutor {
public:
    QueryExecutor(SchemaCatalog&);

    void execute_script(const ASTree*);

private:
    SchemaCatalog& schema_catalog;

    void execute_query(const ASTree*);
    void execute_select(const ASTree*);
    void execute_create(const ASTree*);
    void execute_insert(const ASTree*);
    void execute_update(const ASTree*);
    void execute_delete(const ASTree*);
    void execute_drop(const ASTree*);

};

#endif