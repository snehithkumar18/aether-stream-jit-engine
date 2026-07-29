#ifndef AETHER_DSL_OPERATORS_H
#define AETHER_DSL_OPERATORS_H

#include <stddef.h>
#include <stdint.h>

typedef struct OperatorDef {
    char* name;
    int precedence;
    int is_left_associative;
} OperatorDef;

OperatorDef* operator_create(const char* name, int precedence, int is_left_associative);
void operator_destroy(OperatorDef* op);
int operator_apply(OperatorDef* op, int64_t left, int64_t right);

#endif
