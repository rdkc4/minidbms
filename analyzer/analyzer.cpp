#include "analyzer.hpp"

#include <format>
#include <memory>
#include <optional>
#include <stdexcept>
#include <unordered_set>

Analyzer::Analyzer(const SchemaCatalog& schema_catalog) : schema_catalog{ schema_catalog } {} 

void Analyzer::analyze_script(const ASTree* script) const {
    for(const auto& query : script->get_children()){
        analyze_query(query.get());
    }
}

void Analyzer::analyze_query(const ASTree* query) const {
    switch(query->get_token().token_type){
        case TokenType::SELECT:
            analyze_select(query);
            break;
        case TokenType::CREATE:
            analyze_create(query);
            break;
        case TokenType::INSERT:
            analyze_insert(query);
            break;
        case TokenType::UPDATE:
            analyze_update(query);
            break;
        case TokenType::DELETE:
            analyze_delete(query);
            break;
        case TokenType::DROP:
            analyze_drop(query);
            break;
        default:
            throw std::runtime_error(std::format("Invalid query command: '{}'\n", token_type_str.at(query->get_token().token_type)));
    }
}

void Analyzer::analyze_select(const ASTree* select) const {
    const std::string& table_name = select->child_at(1)->get_token().value;
    analyze_table(table_name);
    
    auto table_schema = schema_catalog.get_table(table_name).value().get();
    analyze_columns(table_schema, select->child_at(0));
    duplicate_columns(select->child_at(0));

    if(select->children_size() >= 3 && select->child_at(2)->get_type() == ASTNodeType::CONDITIONS){
        analyze_conditions(table_schema, select->child_at(2));
    }

    if(select->get_children().back()->get_type() == ASTNodeType::ORDERBY){
        analyze_orderby(table_schema, select->get_children().back().get());
    }
}

void Analyzer::analyze_create(const ASTree* create) const {
    const std::string& table_name = create->child_at(0)->get_token().value;
    analyze_table(table_name, false);
    duplicate_columns(create->child_at(1));
    analyze_keys(create->child_at(1));
}

void Analyzer::analyze_insert(const ASTree* insert) const {
    const std::string& table_name = insert->child_at(0)->get_token().value;
    analyze_table(table_name);

    auto table_schema = schema_catalog.get_table(table_name).value().get();
    analyze_columns(table_schema, insert->child_at(1));
    duplicate_columns(insert->child_at(1));

    if(insert->child_at(1)->children_size() != insert->child_at(2)->children_size()){
        throw std::runtime_error("Number of provided values doesn't match the number of provided columns\n");
    }
    analyze_insert_types(table_schema, insert->child_at(1), insert->child_at(2));
}

void Analyzer::analyze_insert_types(const TableSchema& table_schema, const ASTree* columns, const ASTree* values) const {
    const size_t n = columns->children_size();
    for(size_t i = 0; i < n; ++i){
        const DataType& column_type = table_schema.get_column(columns->child_at(i)->get_token().value)->get().type;
        const DataType& literal_type = literal_to_type.at(values->child_at(i)->get_token().token_type);
        if(column_type != literal_type){
            throw std::runtime_error(std::format("Type mismatch: expected '{}' got '{}'", 
                data_type_str.at(column_type), data_type_str.at(literal_type)));
        }
    } 
}

void Analyzer::analyze_update(const ASTree* update) const {
    const std::string& table_name = update->child_at(0)->get_token().value;
    analyze_table(table_name);

    auto table_schema = schema_catalog.get_table(table_name).value().get();
    analyze_assignments(table_schema, update->child_at(1));

    if(update->children_size() > 2){
        analyze_conditions(table_schema, update->child_at(2));
    }
}

void Analyzer::analyze_delete(const ASTree* _delete) const {
    const std::string& table_name = _delete->child_at(0)->get_token().value;
    analyze_table(table_name);

    if(_delete->children_size() > 1){
        TableSchema table_schema = schema_catalog.get_table(table_name)->get(); 
        analyze_conditions(table_schema, _delete->child_at(1));
    }
}

void Analyzer::analyze_drop(const ASTree* drop) const {
    const std::string& table_name = drop->child_at(0)->get_token().value;
    analyze_table(table_name);
}

void Analyzer::analyze_conditions(const TableSchema& table_schema, const ASTree* conditions) const {
    auto condition = conditions->child_at(0);
    const DataType& left = condition->child_at(0)->get_type() == ASTNodeType::ID ? 
            table_schema.get_column(condition->child_at(0)->get_token().value)->get().type : 
                literal_to_type.at(condition->child_at(0)->get_token().token_type);
    
    const DataType& right = condition->child_at(1)->get_type() == ASTNodeType::ID ? 
            table_schema.get_column(condition->child_at(1)->get_token().value)->get().type : 
                literal_to_type.at(condition->child_at(1)->get_token().token_type);

    if(left != right){
        throw std::runtime_error(std::format("Type mismatch: left op - '{}', right op - '{}'\n", data_type_str.at(left), data_type_str.at(right)));
    }
}

void Analyzer::analyze_orderby(const TableSchema& table_schema, const ASTree* orderby) const {
    analyze_column(table_schema, orderby);
}

void Analyzer::analyze_table(const std::string& table_name, bool should_exist) const {
    bool exists = schema_catalog.get_table(table_name) != std::nullopt;
    if(exists != should_exist){
        throw std::runtime_error(std::format("Table '{}' {}\n", table_name, should_exist ? "doesn't exist" : "already exists"));
    }
}

void Analyzer::analyze_assignments(const TableSchema& table_schema, const ASTree* assignments) const {
    analyze_columns(table_schema, assignments);
    analyze_assignment_type(table_schema, assignments);
}

void Analyzer::analyze_assignment_type(const TableSchema& table_schema, const ASTree* assignments) const {
    for(const auto& column : assignments->get_children()){
        const DataType& column_type = table_schema.get_column(column->get_token().value)->get().type;
        const DataType& literal_type = literal_to_type.at(column->child_at(0)->get_token().token_type);
        if(column_type != literal_type){
            throw std::runtime_error(std::format("Type mismatch: expected '{}' got '{}'", 
                data_type_str.at(column_type), data_type_str.at(literal_type)));
        }
    }
}

void Analyzer::analyze_columns(const TableSchema& table_schema, const ASTree* columns) const {
    if(columns->child_at(0)->get_token().token_type == TokenType::ASTERISK){
        return;
    }
    for(const auto& column : columns->get_children()){
        analyze_column(table_schema, column.get());
    }
}

void Analyzer::analyze_column(const TableSchema& table_schema, const ASTree* column) const {
    if(!table_schema.column_exists(column->get_token().value)){
        throw std::runtime_error(std::format("Unknown column '{}'\n", column->get_token().value));
    }
}

void Analyzer::duplicate_columns(const ASTree* columns) const {
    std::unordered_set<std::string> column_names;
    for(const auto& column : columns->get_children()){
        const std::string& column_name = column->get_token().value;
        if(column_names.find(column_name) != column_names.end()){
            throw std::runtime_error(std::format("Duplicate column '{}'\n", column_name));
        }
        column_names.insert(column_name);
    }
}

void Analyzer::analyze_keys(const ASTree* columns) const {
    unsigned keys_number = 0;
    for(const auto& column : columns->get_children()) {
        if(column->get_children().back()->get_type() == ASTNodeType::KEY) {
            ++keys_number;
        }
    }
    if(keys_number == 0) {
        throw std::runtime_error(std::format("Table has no keys\n"));
    }
    else if(keys_number > 1){
        throw std::runtime_error("Table has multiple keys\n");
    }
}
