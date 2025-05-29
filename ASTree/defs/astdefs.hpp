#ifndef ASTDEFS_HPP
#define ASTDEFS_HPP

#include <unordered_map>
#include <string>

enum class ASTNodeType { SCRIPT, QUERY, SOURCE, CONDITIONS, CONDITION, ORDERBY, COLUMNS, COLUMN, TYPE, ID, ASSIGNMENTS, VALUES, VALUE };

extern const std::unordered_map<ASTNodeType, std::string> ast_node_str;

#endif