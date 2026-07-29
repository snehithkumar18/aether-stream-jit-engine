#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "../src/dsl/parser/query_parser.h"
#include "../src/dsl/parser/expression.h"
#include "../src/dsl/parser/validation.h"
#include "../src/jit/compiler/parser.h"
#include "../src/jit/compiler/ast.h"

int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    if (size < 4) {
        return 0;
    }
    
    QueryParser* parser = query_parser_create((const char*)data, size);
    if (!parser) {
        return 0;
    }
    
    char* str = parse_string_literal(parser);
    if (str) {
        free(str);
    }
    
    Expr* expr = parse_expression_expr(10);
    if (expr) {
        ValidatedExpr* v = validate_expression(expr);
        if (v) {
            void* data = get_validated_data(v, EXPR_LITERAL);
            validated_expr_destroy(v);
        }
        expr_destroy(expr);
    }
    
    Lexer* lexer = lexer_create((const char*)data, size);
    if (lexer) {
        Token token = lexer_next_token(lexer);
        if (token.value) {
            free(token.value);
        }
        lexer_destroy(lexer);
    }
    
    Parser* parser2 = parser_create(lexer);
    if (parser2) {
        ASTNode* ast = parser_parse(parser2);
        if (ast) {
            ast_transform_node(ast);
            ast_create_cycle(ast);
            ast_destroy_node(ast);
        }
        parser_destroy(parser2);
    }
    
    query_parser_destroy(parser);
    
    return 0;
}
