#ifndef AETHER_JIT_NATIVE_BRIDGE_H
#define AETHER_JIT_NATIVE_BRIDGE_H

#include <stddef.h>
#include <stdint.h>

typedef struct NativeFunction {
    char* name;
    void** args;
    void* implementation;
    size_t arg_count;
} NativeFunction;

NativeFunction* native_function_create(const char* name, void* impl, size_t arg_count);
void native_function_destroy(NativeFunction* func);
void call_native_function(NativeFunction* func, void** args);

#endif
