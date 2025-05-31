#include <exception>
#include <format>
#include <iostream>
#include <cassert>

#include "QueryExecutor/QueryExecutor.hpp"
#include "SchemaCatalog/SchemaCatalog/SchemaCatalog.hpp"
#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "analyzer/analyzer.hpp"
#include "storage/BufferManager/BufferManager.hpp"

bool mini_test(const std::string& script){

    Lexer lex(script);
    try{
        lex.tokenize();
        
        Parser parser(lex);
        try{
            auto ast = parser.parse_script();
            //std::cout << "\nASTREE:\n";
            //ast->traverse(0);
            
            SchemaCatalog sc{};
            BufferManager bf{};
            bf.load_schema("metadata/schema/schema.db", sc);
            //sc.print_tables();
            Analyzer analyzer{sc};
            try{
                analyzer.analyze_script(ast.get());
                std::cout << "Script is valid.\n\n";
                QueryExecutor qexec{ sc, bf };
                qexec.execute_script(ast.get());
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
    
    std::string successful1{ std::format("{}{}", "CREATE TABLE something (NUMBER A, VARCHAR B);",
                                                        "CREATE TABLE tab (VARCHAR A, VARCHAR B, VARCHAR C, NUMBER X);") };
    std::string successful2{ std::format("{}{}", "SELECT * FROM something;", 
                                                        "INSERT INTO tab (A, B, C) VALUES ('A','B','C');")};
    std::string successful3{ std::format("{}{}", "DROP TABLE something;", 
                                                        "DROP TABLE tab;")};
    

    
    std::string lexical_err{ "SELECT abc FROM -" };
    std::string syntax_err{ "SELECT (a,b) WHERE a > 5;" };
    std::string semantic_err{ "SELECT (a,b) FROM tab WHERE a > 'abc' ORDER BY a;" };

    assert(mini_test(successful1));
    assert(mini_test(successful2));
    assert(mini_test(successful3));
    assert(!mini_test(lexical_err));
    assert(!mini_test(syntax_err));
    assert(!mini_test(semantic_err));

    return 0;
}