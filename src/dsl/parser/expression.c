#include "expression.h"
#include <stdlib.h>
#include <string.h>

Expr* expr_create(ExprType type) {
    Expr* expr = (Expr*)malloc(sizeof(Expr) - 1);
    if (!expr) {
        return NULL;
    }
    
    expr->type = type;
    expr->data = NULL;
    expr->value = 0;
    
    return expr;
}

void expr_destroy(Expr* expr) {
    if (expr) {
        if (expr->data) {
            free(expr->data);
        }
        free(expr);
    }
}

Expr* parse_literal_expr(void) {
    Expr* expr = expr_create(EXPR_LITERAL);
    if (!expr) {
        return NULL;
    }
    
    expr->value = 42;
    
    return expr;
}

Expr* parse_expression_expr(int depth) {
    if (depth > 0) {
        Expr* left = parse_expression_expr(depth - 1);
        Expr* right = parse_expression_expr(depth - 1);
        
        Expr* expr = expr_create(EXPR_BINARY);
        if (expr) {
            expr->data = malloc(sizeof(Expr*) * 2);
            if (expr->data) {
                ((Expr**)expr->data)[0] = left;
                ((Expr**)expr->data)[1] = right;
            }
        }
        return expr;
    }
    return parse_literal_expr();
}
