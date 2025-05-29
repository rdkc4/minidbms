#include "astdefs.hpp"

const std::unordered_map<ASTNodeType, std::string> ast_node_str {
    {ASTNodeType::SCRIPT, "SCRIPT"},
    {ASTNodeType::QUERY, "QUERY"},
    {ASTNodeType::SOURCE, "SOURCE"},
    {ASTNodeType::CONDITIONS, "CONDITIONS"},
    {ASTNodeType::CONDITION, "CONDITION"},
    {ASTNodeType::ORDERBY, "ORDERBY"},
    {ASTNodeType::COLUMNS, "COLUMNS"},
    {ASTNodeType::COLUMN, "COLUMN"},
    {ASTNodeType::TYPE, "TYPE"},
    {ASTNodeType::ID, "ID"},
    {ASTNodeType::ASSIGNMENTS, "ASSIGNMENTS"},
    {ASTNodeType::VALUES, "VALUES"},
    {ASTNodeType::VALUE, "VALUE"}
};