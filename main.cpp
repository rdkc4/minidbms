#include <exception>
#include <format>
#include <iostream>

#include "BTree/BTree.hpp"
#include "SchemaCatalog/SchemaCatalog/SchemaCatalog.hpp"
#include "SchemaCatalog/TableSchema/TableSchema.hpp"
#include "SchemaCatalog/defs/schemadefs.hpp"
#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "analyzer/analyzer.hpp"

void mini_test(const std::string& script){
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
                std::cout << "Script is valid.\n";
            }
            catch(const std::exception& ex) {
                std::cerr << std::format("Semantic check failed:\n\t{}\n", ex.what());
            }
        } catch(const std::exception& ex) {
            std::cerr << std::format("Syntax check failed:\n\t{}\n", ex.what());
        }
    } catch(const std::exception& ex) {
        std::cerr << std::format("Lexical check failed:\n\t{}\n", ex.what());
    }
    std::cout << "---------------------------------------------\n";
}

int main(){
    const int t = 4;
    static_assert(t > 1, "B-Tree minimum degree T must be greater than 1");
    BTree<int, int, 4> bt;

    bt.insert(10, 10);
    bt.insert(20, 20);
    bt.insert(5, 5);
    bt.insert(6, 6);
    bt.insert(12, 12);
    bt.insert(30, 30);
    bt.insert(7, 7);
    bt.insert(17, 17);

    /* BTree 
    bt.traverse();
    std::cout << "\n";

    int k = 6;
    {
        auto [ptr, idx] = bt.search(k);
        if(ptr == nullptr){
            std::cout << std::format("Key {} not found\n", k);
        }
        else {
            std::cout << std::format("Key {} found, value: {}\n", k, ptr->values[idx]);
        }
    }
    
    k = 15;
    {
        auto [ptr, idx] = bt.search(k);
        if(ptr == nullptr){
            std::cout << std::format("Key {} not found!\n", k);
        }
        else {
            std::cout << std::format("Key {} found! Value: {}\n", k, ptr->values[idx]);
        }
    }
    */

    std::string successful = std::format("{}{}{}{}{}", "SELECT (a,b) FROM tab WHERE a > 5 ORDER BY a;",
                                                            "CREATE TABLE something (NUMBER A, VARCHAR B);",
                                                            "INSERT INTO tab (a, b) VALUES (1, 'f');",
                                                            "UPDATE tab SET a=123, b='agg' WHERE a>2;",
                                                            "DELETE FROM tab WHERE a>0;",
                                                            "DROP TABLE tab;");
    
    std::string lexical_err = std::format("{}", "SELECT abc FROM -");
    std::string syntax_err = std::format("{}", "SELECT (a,b) WHERE a > 5;");
    std::string semantic_err = std::format("{}", "SELECT (a,b) FROM tab WHERE a > 'abc' ORDER BY a;");
    
    mini_test(successful);
    mini_test(lexical_err);
    mini_test(syntax_err);
    mini_test(semantic_err);

    return 0;
}