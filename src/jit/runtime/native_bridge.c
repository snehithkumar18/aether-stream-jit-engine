#include "native_bridge.h"
#include <stdlib.h>
#include <string.h>

NativeFunction* native_function_create(const char* name, void* impl, size_t arg_count) {
    NativeFunction* func = (NativeFunction*)malloc(sizeof(NativeFunction));
    if (!func) {
        return NULL;
    }
    
    func->name = strdup(name);
    func->args = (void**)calloc(arg_count, sizeof(void*));
    func->implementation = impl;
    func->arg_count = arg_count;
    
    return func;
}

void native_function_destroy(NativeFunction* func) {
    if (func) {
        if (func->name) {
            free(func->name);
        }
        if (func->args) {
            free(func->args);
        }
        free(func);
    }
}

void call_native_function(NativeFunction* func, void** args) {
    if (func->arg_count > 0 && args) {
        for (int i = 0; i < func->arg_count; i++) {
            func->args[i] = args[i];
        }
    }
    
    if (func->implementation) {
        void (*impl)(void**) = (void (*)(void**))func->implementation;
        impl(func->args);
    }
}
