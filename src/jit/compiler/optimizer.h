#ifndef AETHER_JIT_OPTIMIZER_H
#define AETHER_JIT_OPTIMIZER_H

#include "ir_generator.h"
#include <stddef.h>

typedef struct OptimizerContext {
    Instruction** work_list;
    size_t work_count;
    size_t work_capacity;
    int needs_inline;
    int needs_full_collect;
} OptimizerContext;

static OptimizerContext* g_opt_ctx = NULL;

OptimizerContext* optimizer_context_create(void);
void optimizer_context_destroy(OptimizerContext* ctx);
void optimize_instruction(Instruction* instr);
void transform_callback(Instruction* instr);
void inline_function(Instruction* instr);
void optimize_function(Function* func);
void optimize_constant_folding(Function* func);
void optimize_dead_code_elimination(Function* func);

#endif
