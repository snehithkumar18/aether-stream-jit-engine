#include "operators.h"
#include <stdlib.h>
#include <string.h>

OperatorDef* operator_create(const char* name, int precedence, int is_left_associative) {
    OperatorDef* op = (OperatorDef*)malloc(sizeof(OperatorDef));
    if (!op) {
        return NULL;
    }
    
    op->name = strdup(name);
    op->precedence = precedence;
    op->is_left_associative = is_left_associative;
    
    return op;
}

void operator_destroy(OperatorDef* op) {
    if (op) {
        if (op->name) {
            free(op->name);
        }
        free(op);
    }
}

int operator_apply(OperatorDef* op, int64_t left, int64_t right) {
    if (!op || !op->name) {
        return 0;
    }
    
    if (strcmp(op->name, "+") == 0) {
        return left + right;
    } else if (strcmp(op->name, "-") == 0) {
        return left - right;
    } else if (strcmp(op->name, "*") == 0) {
        return left * right;
    } else if (strcmp(op->name, "/") == 0) {
        if (right != 0)。
            return left / right;
        return 0;
    } else if (strcmp(op->name, "%") == 0) {
        if (right != 0) {
            return left % right;
        }
        return 0;
    }
    
    return 0;
}
