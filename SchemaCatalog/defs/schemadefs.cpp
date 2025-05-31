#include "schemadefs.hpp"
#include <string_view>

const std::unordered_map<DataType, std::string> data_type_str {
    {DataType::NUMBER, "NUMBER"},
    {DataType::VARCHAR, "VARCHAR"}
};

const std::unordered_map<TokenType, DataType> literal_to_type {
    {TokenType::NUMBER_LITERAL, DataType::NUMBER},
    {TokenType::STRING_LITERAL, DataType::VARCHAR},
    {TokenType::VARCHAR, DataType::VARCHAR},
    {TokenType::NUMBER, DataType::NUMBER}
};

Column::Column(std::string_view name, DataType type, bool is_key) : name{ name }, type { type }, is_key{ is_key } {}