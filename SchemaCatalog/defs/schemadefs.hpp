#ifndef SCHEMADEFS_HPP
#define SCHEMADEFS_HPP

#include <string>

enum class DataType { NUMBER, VARCHAR };

struct Column {
    std::string name;
    DataType type;

    Column(std::string_view, DataType);
};

#endif