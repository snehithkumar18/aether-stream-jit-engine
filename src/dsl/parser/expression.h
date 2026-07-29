#ifndef AETHER_DSL_EXPRESSION_H
#define AETHER_DSL_EXPRESSION_H

#include <stddef.h>
#include <stdint.h>

typedef enum ExprType {
    EXPR_LITERAL,
    EXPR_IDENTIFIER,
    EXPR_BINARY,
    EXPR_UNARY,
    EXPR_CALL
} ExprType;

typedef struct Expr {
    ExprType type;
    void* data;
    int64_t value;
} Expr;

Expr* expr_create(ExprType type);
void expr_destroy(Expr* expr);
Expr* parse_expression_expr(int depth);
Expr* parse_literal_expr(void);

#endif
