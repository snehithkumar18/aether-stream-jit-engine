#ifndef AETHER_DSL_VALIDATION_H
#define AETHER_DSL_VALIDATION_H

#include <stddef.h>
#include <stdint.h>

typedef struct ValidatedExpr {
    ExprType type;
    void* data;
} ValidatedExpr;

ValidatedExpr* validate_expression(Expr* expr);
void* get_validated_data(ValidatedExpr* v, ExprType expected);
void validated_expr_destroy(ValidatedExpr* v);

#endif
