#include "QueryExecutor.hpp"

QueryExecutor::QueryExecutor(SchemaCatalog& schema_catalog, BufferManager& buffer_manager) : schema_catalog{ schema_catalog }, buffer_manager{buffer_manager} {}

void QueryExecutor::execute_script(const ASTree* script) {
    for(const auto& query : script->get_children()){
        execute_query(query.get());
    }
}

void QueryExecutor::execute_query(const ASTree* query) {
    switch(query->get_token().token_type) {
        case TokenType::SELECT:
            execute_select(query);
            break;
        case TokenType::CREATE:
            execute_create(query);
            break;
        case TokenType::INSERT:
            execute_insert(query);
            break;
        case TokenType::UPDATE:
            execute_update(query);
            break;
        case TokenType::DELETE:
            execute_delete(query);
            break;
        case TokenType::DROP:
            execute_drop(query);
            break;
        default:
            break;
    }
}

void QueryExecutor::execute_select(const ASTree* select) {
    (void)select;
}

void QueryExecutor::execute_create(const ASTree* create) {
    TableSchema table_schema{create->child_at(0)->get_token().value};
    for(const auto& column : create->child_at(1)->get_children()){
        DataType type = column->child_at(0)->get_token().token_type == TokenType::VARCHAR ? DataType::VARCHAR : DataType::NUMBER;
        Column col{column->get_token().value, type};
        table_schema.add_column(col);
    }
    buffer_manager.save_schema("metadata/schema/schema.db", table_schema);
}

void QueryExecutor::execute_insert(const ASTree* insert) {
    (void)insert;
}

void QueryExecutor::execute_update(const ASTree* update) {
    (void)update;
}

void QueryExecutor::execute_delete(const ASTree* _delete) {
    (void)_delete;
}

void QueryExecutor::execute_drop(const ASTree* drop) {
    buffer_manager.delete_schema("metadata/schema/schema.db", drop->child_at(0)->get_token().value, schema_catalog);
}