#ifndef SCHEMA_CATALOG_HPP
#define SCHEMA_CATALOG_HPP

#include <string>
#include <unordered_map>
#include <optional>

#include "../TableSchema/TableSchema.hpp"

class SchemaCatalog{
public:

    void add_table(const TableSchema&);
    std::optional<std::reference_wrapper<const TableSchema>> get_table(const std::string&) const noexcept;
    bool table_exists(const std::string&) const noexcept;

private:
    std::unordered_map<std::string, TableSchema> tables;

};

#endif