#ifndef SCHEMADEFS_HPP
#define SCHEMADEFS_HPP

#include <string>
#include <unordered_map>

#include "../../token/defs/tokendefs.hpp"

enum class DataType { NUMBER, VARCHAR };

extern const std::unordered_map<DataType, std::string> data_type_str;

extern const std::unordered_map<TokenType, DataType> literal_to_type;

struct Column {
    std::string name;
    DataType type;

    Column(std::string_view, DataType);
};

#endif