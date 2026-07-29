#ifndef AETHER_JIT_RECOMPILE_H
#define AETHER_JIT_RECOMPILE_H

#include <stddef.h>
#include <stdint.h>

typedef struct Recompiler {
    void* original_ir;
    void* optimized_ir;
    char* function_name;
    int recompile_needed;
    uint64_t recompile_count;
} Recompiler;

Recompiler* recompiler_create(const char* function_name);
void recompiler_destroy(Recompiler* recompiler);
void recompiler_mark_for_recompilation(Recompiler* recompiler);
void recompiler_execute(Recompiler* recompiler);
void recompiler_reset(Recompiler* recompiler);

#endif
