#include "parser.hpp"
#include <format>
#include <memory>
#include <stdexcept>

Parser::Parser(Lexer& lex) : lexer{ lex }, token_idx{ 0 } {} 

std::unique_ptr<ASTree> Parser::parse_script() {
    std::unique_ptr<ASTree> script = std::make_unique<ASTree>(ASTree{Token{}, ASTNodeType::SCRIPT});
    token = token_at(token_idx);
    while(token.token_type != TokenType::END){
        script->add_child(parse_query());
    }
    return script;
}

const Token& Parser::token_at(size_t n) const noexcept {
    return lexer.token_at(n);
}

const Token& Parser::next_token() noexcept {
    return token_at(++token_idx);
}

const Token& Parser::peek() const noexcept {
    return token_at(token_idx + 1);
}

void Parser::consume_token(TokenType token_type){
    if(token.token_type != token_type){
        throw std::runtime_error(std::format("Expected {}, got {}\n", token_type_str.at(token_type), token_type_str.at(token.token_type)));
    }
    token = next_token();
}

std::unique_ptr<ASTree> Parser::parse_query(){
    switch(token.token_type) {
        case TokenType::SELECT:
            return parse_select();
        case TokenType::CREATE:
            return parse_create();
        case TokenType::INSERT:
            return parse_insert();
        case TokenType::UPDATE:
            return parse_update();
        case TokenType::DELETE:
            return parse_delete();
        case TokenType::DROP:
            return parse_drop();
        default:
            throw std::runtime_error(std::format("Invalid query command: {}\n", token_type_str.at(token.token_type)));
    }
}

std::unique_ptr<ASTree> Parser::parse_select(){
    std::unique_ptr<ASTree> select_query = std::make_unique<ASTree>(token, ASTNodeType::QUERY);

    consume_token(TokenType::SELECT);
    select_query->add_child(parse_select_columns());
    
    consume_token(TokenType::FROM);
    select_query->add_child(parse_id());
    
    if(token.token_type == TokenType::WHERE){
        select_query->add_child(parse_condition());
    }
    if(token.token_type == TokenType::ORDER){
        select_query->add_child(parse_orderby());
    }

    consume_token(TokenType::SEMICOLON);
    return select_query;
}

std::unique_ptr<ASTree> Parser::parse_create(){
    std::unique_ptr<ASTree> create_query = std::make_unique<ASTree>(token, ASTNodeType::QUERY);
    consume_token(TokenType::CREATE);
    consume_token(TokenType::TABLE);
    
    create_query->add_child(parse_id());
    create_query->add_child(parse_table_columns());
    
    consume_token(TokenType::SEMICOLON);
    return create_query;
}

std::unique_ptr<ASTree> Parser::parse_insert(){
    std::unique_ptr<ASTree> insert_query = std::make_unique<ASTree>(token, ASTNodeType::QUERY);
    consume_token(TokenType::INSERT);
    consume_token(TokenType::INTO);
    
    insert_query->add_child(parse_id());
    insert_query->add_child(parse_columns());
    insert_query->add_child(parse_values());
    
    consume_token(TokenType::SEMICOLON); 
    return insert_query;
}

std::unique_ptr<ASTree> Parser::parse_update(){
    std::unique_ptr<ASTree> update_query = std::make_unique<ASTree>(token, ASTNodeType::QUERY);
    consume_token(TokenType::UPDATE);
    update_query->add_child(parse_id());

    consume_token(TokenType::SET);
    update_query->add_child(parse_assignments());

    if(token.token_type == TokenType::WHERE){
        update_query->add_child(parse_condition());
    }

    consume_token(TokenType::SEMICOLON);
    return update_query;
}

std::unique_ptr<ASTree> Parser::parse_delete(){
    std::unique_ptr<ASTree> delete_query = std::make_unique<ASTree>(token, ASTNodeType::QUERY);
    consume_token(TokenType::DELETE);

    consume_token(TokenType::FROM);
    delete_query->add_child(parse_id());

    if(token.token_type == TokenType::WHERE){
        delete_query->add_child(parse_condition());
    }

    consume_token(TokenType::SEMICOLON);
    return delete_query;
}

std::unique_ptr<ASTree> Parser::parse_drop(){
    std::unique_ptr<ASTree> drop_query = std::make_unique<ASTree>(token, ASTNodeType::QUERY);
    consume_token(TokenType::DROP);

    consume_token(TokenType::TABLE);
    drop_query->add_child(parse_id());

    consume_token(TokenType::SEMICOLON);
    return drop_query;
}

std::unique_ptr<ASTree> Parser::parse_select_columns(){
    std::unique_ptr<ASTree> columns = std::make_unique<ASTree>(Token{}, ASTNodeType::COLUMNS);
    if(token.token_type == TokenType::ASTERISK){
        columns->add_child(std::make_unique<ASTree>(token, ASTNodeType::COLUMN));
        consume_token(TokenType::ASTERISK);
    }
    else if(token.token_type == TokenType::ID){
        columns->add_child(std::make_unique<ASTree>(token, ASTNodeType::COLUMN));
        consume_token(TokenType::ID);
    }
    else{
        consume_token(TokenType::LPAREN);

        while(token.token_type == TokenType::ID){
            columns->add_child(std::make_unique<ASTree>(token, ASTNodeType::COLUMN));
            consume_token(TokenType::ID);
            if(token.token_type == TokenType::COMMA){
                consume_token(TokenType::COMMA);
            }
            else{
                break;
            }
        }
        consume_token(TokenType::RPAREN);
    }
    return columns;
}

std::unique_ptr<ASTree> Parser::parse_columns(){
    consume_token(TokenType::LPAREN);
    std::unique_ptr<ASTree> columns = std::make_unique<ASTree>(Token{}, ASTNodeType::COLUMNS);
    
    while(token.token_type == TokenType::ID){
        columns->add_child(std::make_unique<ASTree>(token, ASTNodeType::COLUMN));   
        consume_token(TokenType::ID);
        if(token.token_type == TokenType::COMMA){
            consume_token(TokenType::COMMA);
        }
        else{
            break;
        }
    }
    consume_token(TokenType::RPAREN);
    return columns;
}

std::unique_ptr<ASTree> Parser::parse_condition(){
    std::unique_ptr<ASTree> conditions = std::make_unique<ASTree>(token, ASTNodeType::CONDITIONS);
    consume_token(TokenType::WHERE);
    
    std::unique_ptr<ASTree> lchild = token.general_type == GeneralTokenType::LITERAL ? parse_value() : parse_id();
    std::unique_ptr<ASTree> condition = std::make_unique<ASTree>(token, ASTNodeType::CONDITION);
    consume_token(token.general_type == GeneralTokenType::OPERATOR ? token.token_type : TokenType::NONE);
    condition->add_child(std::move(lchild));
    condition->add_child(token.general_type == GeneralTokenType::LITERAL ? parse_value() : parse_id());

    return conditions;
}

std::unique_ptr<ASTree> Parser::parse_orderby(){
    consume_token(TokenType::ORDER);
    consume_token(TokenType::BY);

    std::unique_ptr<ASTree> orderby = std::make_unique<ASTree>(token, ASTNodeType::ORDERBY);
    consume_token(TokenType::ID);

    return orderby;
}

std::unique_ptr<ASTree> Parser::parse_table_columns(){
    consume_token(TokenType::LPAREN);

    std::unique_ptr<ASTree> defs = std::make_unique<ASTree>(Token{}, ASTNodeType::COLUMNS);
    while(token.general_type == GeneralTokenType::TYPE){
        defs->add_child(std::make_unique<ASTree>(token, ASTNodeType::TYPE));
        consume_token(token.token_type);
        
        defs->add_child(std::make_unique<ASTree>(token, ASTNodeType::COLUMN));
        consume_token(TokenType::ID);
        
        if(token.token_type == TokenType::COMMA){
            consume_token(TokenType::COMMA);
        }
        else{
            break;
        }
    }
    consume_token(TokenType::RPAREN);

    return defs;
}

std::unique_ptr<ASTree> Parser::parse_values(){
    consume_token(TokenType::VALUES);
    consume_token(TokenType::LPAREN);
    std::unique_ptr<ASTree> values = std::make_unique<ASTree>(Token{}, ASTNodeType::VALUES);
    while(token.general_type == GeneralTokenType::LITERAL){
        values->add_child(parse_value());
        if(token.token_type == TokenType::COMMA){
            consume_token(TokenType::COMMA);
        }
        else{
            break;
        }
    }

    consume_token(TokenType::RPAREN);
    return values;
}

std::unique_ptr<ASTree> Parser::parse_assignments(){
    std::unique_ptr<ASTree> id = std::make_unique<ASTree>(token, ASTNodeType::ASSIGNMENTS);

    while(token.token_type == TokenType::ID){
        id->add_child(std::make_unique<ASTree>(token, ASTNodeType::COLUMN));
        consume_token(TokenType::ID);
        consume_token(TokenType::EQUAL);
        id->add_child(parse_value());

        if(token.token_type == TokenType::COMMA){
            consume_token(TokenType::COMMA);
        }
        else{
            break;
        }
    }
    return id;
}

std::unique_ptr<ASTree> Parser::parse_id(){
    std::unique_ptr<ASTree> id = std::make_unique<ASTree>(token, ASTNodeType::ID);
    consume_token(TokenType::ID);
    return id;
}

std::unique_ptr<ASTree> Parser::parse_value(){
    std::unique_ptr<ASTree> value = std::make_unique<ASTree>(token, ASTNodeType::VALUE);
    consume_token(token.token_type == TokenType::STRING_LITERAL ? TokenType::STRING_LITERAL : TokenType::NUMBER_LITERAL);
    return value;
}