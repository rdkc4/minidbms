#ifndef ANALYZER_HPP
#define ANALYZER_HPP

#include "../ASTree/ASTree.hpp"
#include "../SchemaCatalog/SchemaCatalog/SchemaCatalog.hpp"

class Analyzer{
public:
    Analyzer(const SchemaCatalog&);

    void analyze_script(const ASTree*) const;

private:
    const SchemaCatalog& schema_catalog;

    void analyze_query(const ASTree*) const;
    void analyze_select(const ASTree*) const;
    void analyze_create(const ASTree*) const;
    void analyze_insert(const ASTree*) const;
    void analyze_update(const ASTree*) const;
    void analyze_delete(const ASTree*) const;
    void analyze_drop(const ASTree*) const;
    void analyze_conditions(const TableSchema&, const ASTree*) const;
    void analyze_orderby(const TableSchema&, const ASTree*) const;

    void analyze_table(const std::string&, bool should_exist = true) const;
    void analyze_assignments(const TableSchema&, const ASTree*) const;
    void analyze_assignment_type(const TableSchema&, const ASTree*) const;

    void analyze_columns(const TableSchema&, const ASTree*) const;
    void analyze_column(const TableSchema&, const ASTree*) const;
    void duplicate_columns(const ASTree*) const;
    void analyze_insert_types(const TableSchema&, const ASTree*, const ASTree*) const;
    void analyze_keys(const ASTree*) const;

};

#endif
