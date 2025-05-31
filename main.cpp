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

enum class Error { LEXICAL_ERR, SYNTAX_ERR, SEMANTIC_ERR, NO_ERR };

Error mini_test(const std::string& script){
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
                return Error::NO_ERR;
            }
            catch(const std::exception& ex) {
                std::cerr << std::format("Semantic check failed:\n\t{}\n", ex.what());
                return Error::SEMANTIC_ERR;
            }
        } catch(const std::exception& ex) {
            std::cerr << std::format("Syntax check failed:\n\t{}\n", ex.what());
            return Error::SYNTAX_ERR;
        }
    } catch(const std::exception& ex) {
        std::cerr << std::format("Lexical check failed:\n\t{}\n", ex.what());
        return Error::LEXICAL_ERR;
    }
}

int main(){
    
    std::string successful1{ std::format("{}{}", "CREATE TABLE something (PRIMARY KEY NUMBER A, VARCHAR B);",
                                                        "CREATE TABLE tab (VARCHAR A, PRIMARY KEY VARCHAR B, VARCHAR C, NUMBER X);") };
    std::string successful2{ std::format("{}{}", "SELECT * FROM something;", 
                                                        "INSERT INTO tab (A, B, C) VALUES ('A','B','C');")};
    std::string successful3{ std::format("{}{}", "DROP TABLE something;", 
                                                        "DROP TABLE tab;")};
        
    std::string lexical_err{ "SELECT abc FROM -" };
    std::string syntax_err{ "SELECT (a,b) WHERE a > 5;" };
    std::string semantic_err1{ "SELECT (a,b) FROM tab WHERE a > 'abc' ORDER BY a;" };
    std::string semantic_err2{ "CREATE TABLE tmp (VARCHAR A, VARCHAR B);"};
    std::string semantic_err3{ "CREATE TABLE tmp (PRIMARY KEY VARCHAR A, PRIMARY KEY VARCHAR B);"};

    assert(mini_test(successful1) == Error::NO_ERR);
    assert(mini_test(successful2) == Error::NO_ERR);
    assert(mini_test(successful3) == Error::NO_ERR);
    assert(mini_test(lexical_err) == Error::LEXICAL_ERR);
    assert(mini_test(syntax_err) == Error::SYNTAX_ERR);
    assert(mini_test(semantic_err1) == Error::SEMANTIC_ERR);
    assert(mini_test(semantic_err2) == Error::SEMANTIC_ERR);
    assert(mini_test(semantic_err3) == Error::SEMANTIC_ERR);

    return 0;
}