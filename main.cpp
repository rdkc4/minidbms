#include <exception>
#include <format>
#include <iostream>
#include <cassert>

//#include "BTree/BTree.hpp"
#include "SchemaCatalog/SchemaCatalog/SchemaCatalog.hpp"
#include "SchemaCatalog/TableSchema/TableSchema.hpp"
#include "SchemaCatalog/defs/schemadefs.hpp"
#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "analyzer/analyzer.hpp"

bool mini_test(const std::string& script){
    std::vector<Column> cols = {Column{"a", DataType::NUMBER}, Column{"b", DataType::VARCHAR}};    
    TableSchema ts{"tab"};
    for(const auto& col : cols){
        ts.add_column(col);
    }
    SchemaCatalog sc{};
    sc.add_table(ts);

    Lexer lex(script);
    try{
        lex.tokenize();
        
        Parser parser(lex);
        try{
            auto ast = parser.parse_script();
            //std::cout << "\nASTREE:\n";
            //ast->traverse(0);

            Analyzer analyzer{sc};
            try{
                analyzer.analyze_script(ast.get());
                std::cout << "Script is valid.\n\n";
                return true;
            }
            catch(const std::exception& ex) {
                std::cerr << std::format("Semantic check failed:\n\t{}\n", ex.what());
                return false;
            }
        } catch(const std::exception& ex) {
            std::cerr << std::format("Syntax check failed:\n\t{}\n", ex.what());
            return false;
        }
    } catch(const std::exception& ex) {
        std::cerr << std::format("Lexical check failed:\n\t{}\n", ex.what());
        return false;
    }
}

int main(){
    /*
    const int t = 4;
    static_assert(t > 1, "B-Tree minimum degree T must be greater than 1");
    BTree<int, int, 4> bt;
    */
    
    std::string successful{ std::format("{}{}{}{}{}", "SELECT (a,b) FROM tab WHERE a > 5 ORDER BY a;",
                                                            "CREATE TABLE something (NUMBER A, VARCHAR B);",
                                                            "INSERT INTO tab (a, b) VALUES (1, 'f');",
                                                            "UPDATE tab SET a=123, b='agg' WHERE a>2;",
                                                            "DELETE FROM tab WHERE a>0;",
                                                            "DROP TABLE tab;") };
    
    std::string lexical_err{ "SELECT abc FROM -" };
    std::string syntax_err{ "SELECT (a,b) WHERE a > 5;" };
    std::string semantic_err{ "SELECT (a,b) FROM tab WHERE a > 'abc' ORDER BY a;" };
    
    assert(mini_test(successful));
    assert(!mini_test(lexical_err));
    assert(!mini_test(syntax_err));
    assert(!mini_test(semantic_err));

    return 0;
}