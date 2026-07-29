#ifndef AETHER_DSL_FUNCTIONS_H
#define AETHER_DSL_FUNCTIONS_H

#include <stddef.h>
#include <stdint.h>

typedef struct FunctionDef {
    char* name;
    void* implementation;
    size_t param_count;
} FunctionDef;

FunctionDef* function_create(const char* name, void* impl, size_t param_count);
void function_destroy(FunctionDef* func);
void* function_call(FunctionDef* func, void** args);

#endif
