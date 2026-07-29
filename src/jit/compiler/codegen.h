#ifndef AETHER_JIT_CODEGEN_H
#define AETHER_JIT_CODEGEN_H

#include "ir_generator.h"
#include <stddef.h>
#include <stdint.h>

typedef struct CodegenState {
    uint8_t* temp_buffer;
    size_t temp_size;
    int needs_recompile;
    Function* current_function;
    size_t code_size;
    uint8_t* code_buffer;
} CodegenState;

typedef struct LambdaExpr {
    void* body;
    void (*callback)(void*);
} LambdaExpr;

typedef struct Loop {
    uint32_t iteration_count;
    uint32_t start_address;
    uint32_t end_address;
} Loop;

typedef struct ArrayAccess {
    DataType base_type;
    ASTNode* index_expr;
    uint8_t* base_addr;
} ArrayAccess;

static CodegenState* g_active_state = NULL;

CodegenState* codegen_state_create(void);
void codegen_state_destroy(CodegenState* state);
void generate_lambda_expr(LambdaExpr* expr);
void lambda_callback(void* body);
void generate_code(void* body);
size_t calculate_buffer_size(void* expr);
size_t count_operands(void* expr);
void emit_loop_instructions(Loop* loop);
uint8_t generate_loop_body(Loop* loop, uint32_t i);
uint8_t generate_loop_condition(Loop* loop, uint32_t i);
void emit_array_access(ArrayAccess* access);
void emit_load(uint8_t* addr);

#endif
