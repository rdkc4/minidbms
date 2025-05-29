#ifndef TABLE_SCHEMA_HPP
#define TABLE_SCHEMA_HPP

#include <string>
#include <vector>
#include <optional>

#include "../defs/schemadefs.hpp"

class TableSchema{
public:
    TableSchema(std::string_view);

    void add_column(const Column&);

    const std::string& get_table_name() const noexcept;
    std::optional<std::reference_wrapper<const Column>> get_column(const std::string&) const noexcept;

private:
    std::string table_name;
    std::vector<Column> columns;

};

#endif