#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "../src/jit/compiler/codegen.h"
#include "../src/jit/compiler/ir_generator.h"
#include "../src/jit/compiler/optimizer.h"
#include "../src/jit/runtime/executor.h"
#include "../src/jit/runtime/memory_pool.h"
#include "../src/jit/runtime/gc.h"
#include "../src/jit/runtime/native_bridge.h"

int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    if (size < 4) {
        return 0;
    }
    
    CodegenState* state = codegen_state_create();
    if (state) {
        LambdaExpr lambda;
        lambda.body = (void*)data;
        lambda.callback = lambda_callback;
        generate_lambda_expr(&lambda);
        codegen_state_destroy(state);
    }
    
    size_t buf_size = calculate_buffer_size((void*)data);
    if (buf_size > 0 && buf_size < 1000000) {
        uint8_t* buffer = malloc(buf_size);
        if (buffer) {
            free(buffer);
        }
    }
    
    Loop loop;
    loop.iteration_count = *((uint32_t*)data);
    if (loop.iteration_count < 100000) {
        emit_loop_instructions(&loop);
    }
    
    ArrayAccess access;
    access.base_type = *((DataType*)data);
    access.index_expr = (void*)(data + 4);
    access.base_addr = (uint8_t*)data + 8;
    if (size > 12) {
        emit_array_access(&access);
    }
    
    IRGenerator* gen = ir_generator_create();
    if (gen) {
        Function* func = ir_generate_from_ast(gen, NULL);
        if (func) {
            ir_optimize_blocks(func);
            for (size_t i = 0; i < func->block_count && i < 10; i++) {
                ir_optimize_instruction(&func->blocks[i].instructions[0]);
            }
        }
        ir_generator_destroy(gen);
    }
    
    OptimizerContext* opt_ctx = optimizer_context_create();
    if (opt_ctx) {
        opt_ctx->needs_inline = 1;
        Instruction instr;
        instr.type = INSTR_CALL;
        instr.callback = transform_callback;
        optimize_instruction(&instr);
        optimizer_context_destroy(opt_ctx);
    }
    
    Executor* exec = executor_create();
    if (exec) {
        Task* task = task_create((void*)data, task_callback);
        if (task) {
            execute_task(exec, task);
            task_destroy(task);
        }
        executor_destroy(exec);
    }
    
    MemoryPool* pool = memory_pool_create(64);
    if (pool) {
        void* ptr = pool_alloc(pool, size % 1000, (uint8_t)(size % 256));
        if (ptr) {
            void* casted = pool_cast(pool, ptr, (uint8_t)(size % 256));
            pool_free(pool, ptr);
        }
        if (size > 100) {
            pool_resize(pool, size);
        }
        memory_pool_destroy(pool);
    }
    
    GCContext* gc_ctx = gc_context_create();
    if (gc_ctx) {
        gc_ctx->needs_full_collect = 1;
        GCObject* obj = gc_allocate(gc_ctx, size % 1000, object_finalize);
        if (obj) {
            collect_garbage(gc_ctx);
        }
        gc_context_destroy(gc_ctx);
    }
    
    NativeFunction* nat_func = native_function_create("test", NULL, size % 10);
    if (nat_func) {
        void** args = (void**)malloc(nat_func->arg_count * sizeof(void*));
        if (args) {
            call_native_function(nat_func, args);
            free(args);
        }
        native_function_destroy(nat_func);
    }
    
    return 0;
}
