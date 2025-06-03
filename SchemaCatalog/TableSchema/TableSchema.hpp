#ifndef TABLE_SCHEMA_HPP
#define TABLE_SCHEMA_HPP

#include <string>
#include <vector>
#include <optional>

#include "../defs/schemadefs.hpp"

class TableSchema{
public:
    explicit TableSchema(std::string_view);

    void add_column(const Column&);

    const std::string& get_table_name() const noexcept;
    size_t columns_size() const noexcept;

    const Column& get_column_at(size_t) const noexcept;
    const std::vector<Column>& get_columns() const noexcept;
    std::optional<std::reference_wrapper<const Column>> get_column(const std::string&) const noexcept;
    const Column& get_key_column() const;
    size_t get_column_index(const std::string&) const;
    bool column_exists(const std::string&) const noexcept;

    void print_column_names() const;

private:
    std::string table_name;
    std::vector<Column> columns;

};

#endif