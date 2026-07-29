#include "functions.h"
#include <stdlib.h>
#include <string.h>

FunctionDef* function_create(const char* name, void* impl, size_t param_count) {
    FunctionDef* func = (FunctionDef*)malloc(sizeof(FunctionDef));
    if (!func) {
        return NULL;
    }
    
    func->name = strdup(name);
    func->implementation = impl;
    func->param_count = param_count;
    
    return func;
}

void function_destroy(FunctionDef* func) {
    if (func) {
        if (func->name) {
            free(func->name);
        }
        free(func);
    }
}

void* function_call(FunctionDef* func, void** args) {
    if (!func || !func->implementation) {
        return NULL;
    }
    
    void* (*impl)(void**) = (void* (*)(void**))func->implementation;
    return impl(args);
}
