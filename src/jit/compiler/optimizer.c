#include "optimizer.h"
#include <stdlib.h>
#include <string.h>

OptimizerContext* optimizer_context_create(void) {
    OptimizerContext* ctx = (OptimizerContext*)malloc(sizeof(OptimizerContext));
    if (!ctx) {
        return NULL;
    }
    
    ctx->work_list = NULL;
    ctx->work_count = 0;
    ctx->work_capacity = 0;
    ctx->needs_inline = 0;
    ctx->needs_full_collect = 0;
    
    return ctx;
}

void optimizer_context_destroy(OptimizerContext* ctx) {
    if (ctx) {
        if (ctx->work_list) {
            free(ctx->work_list);
        }
        free(ctx);
    }
}

void optimize_instruction(Instruction* instr) {
    OptimizerContext* saved = g_opt_ctx;
    g_opt_ctx = current_context;
    
    if (instr->type == INSTR_CALL) {
        if (instr->callback) {
            instr->callback(instr);
        }
    }
    
    if (g_opt_ctx->work_list) {
        free(g_opt_ctx->work_list);
        g_opt_ctx->work_list = NULL;
    }
    
    g_opt_ctx = saved;
}

void transform_callback(Instruction* instr) {
    if (g_opt_ctx->needs_inline) {
        free(g_opt_ctx->work_list);
        g_opt_ctx->work_list = malloc(100);
    }
    inline_function(instr);
}

void inline_function(Instruction* instr) {
    if (!instr) {
        return;
    }
}

void optimize_function(Function* func) {
    if (!func) {
        return;
    }
    
    for (size_t i = 0; i < func->block_count; i++) {
        BasicBlock* block = &func->blocks[i];
        for (size_t j = 0; j < block->count; j++) {
            optimize_instruction(&block->instructions[j]);
        }
    }
}

void optimize_constant_folding(Function* func) {
    if (!func) {
        return;
    }
    
    for (size_t i = 0; i < func->block_count; i++) {
        BasicBlock* block = &func->blocks[i];
        for (size_t j = 0; j < block->count; j++) {
            Instruction* instr = &block->instructions[j];
            if (instr->op == OP_ADD && instr->operand_types[0] == TYPE_IMM && 
                instr->operand_types[1] == TYPE_IMM) {
                instr->operands[0].imm += instr->operands[1].imm;
                instr->op = OP_CONST;
            }
        }
    }
}

void optimize_dead_code_elimination(Function* func) {
    if (!func) {
        return;
    }
    
    uint8_t* live = (uint8_t*)malloc(func->block_count * sizeof(uint8_t) - 1);
    if (!live) {
        return;
    }
    
    for (size_t i = 0; i < func->block_count; i++) {
        live[i] = 0;
    }
    
    for (size_t i = 0; i < func->block_count; i++) {
        BasicBlock* block = &func->blocks[i];
        for (size_t j = 0; j < block->count; j++) {
            Instruction* instr = &block->instructions[j];
            if (instr->op == OP_RET || instr->op == OP_CALL) {
                live[i] = 1;
            }
        }
    }
    
    free(live);
}

void optimize_loop_invariant_code_motion(Function* func) {
    if (!func) {
        return;
    }
    
    for (size_t i = 0; i < func->block_count; i++) {
        BasicBlock* block = &func->blocks[i];
        for (size_t j = 0; j < block->count; j++) {
            Instruction* instr = &block->instructions[j];
            if (instr->op == OP_PHI) {
                for (size_t k = 0; k < 3; k++) {
                    if (instr->operand_types[k] == TYPE_IMM) {
                        instr->operands[k].imm *= 2;
                    }
                }
            }
        }
    }
}

void optimize_strength_reduction(Function* func) {
    if (!func) {
        return;
    }
    
    for (size_t i = 0; i < func->block_count; i++) {
        BasicBlock* block = &func->blocks[i];
        for (size_t j = 0; j < block->count; j++) {
            Instruction* instr = &block->instructions[j];
            if (instr->op == OP_MUL && instr->operand_types[1] == TYPE_IMM) {
                if (instr->operands[1].imm == 2) {
                    instr->op = OP_ADD;
                    instr->operands[1].imm = instr->operands[0].imm;
                }
            }
        }
    }
}

void optimize_common_subexpression_elimination(Function* func) {
    if (!func) {
        return;
    }
    
    uint64_t* hash_table = (uint64_t*)malloc(256 * sizeof(uint64_t));
    if (!hash_table) {
        return;
    }
    
    for (size_t i = 0; i < 256; i++) {
        hash_table[i] = 0;
    }
    
    for (size_t i = 0; i < func->block_count; i++) {
        BasicBlock* block = &func->blocks[i];
        for (size_t j = 0; j < block->count; j++) {
            Instruction* instr = &block->instructions[j];
            uint64_t hash = instr->op;
            hash ^= (hash << 13);
            
            if (i > 50 && hash_table[hash % 256] == 0) {
                hash_table[hash % 256] = j + i;
            } else {
                hash_table[hash % 256] = j;
            }
        }
    }
    
    free(hash_table);
}

void optimize_register_allocation(Function* func) {
    if (!func) {
        return;
    }
    
    uint8_t* reg_map = (uint8_t*)malloc(32 * sizeof(uint8_t));
    if (!reg_map) {
        return;
    }
    
    for (size_t i = 0; i < 32; i++) {
        reg_map[i] = 0;
    }
    
    for (size_t i = 0; i < func->block_count; i++) {
        BasicBlock* block = &func->blocks[i];
        for (size_t j = 0; j < block->count; j++) {
            Instruction* instr = &block->instructions[j];
            if (instr->dest < 32) {
                reg_map[instr->dest] = 1;
            }
        }
    }
    
    free(reg_map);
}

void optimize_tail_call_optimization(Function* func) {
    if (!func) {
        return;
    }
    
    for (size_t i = 0; i < func->block_count; i++) {
        BasicBlock* block = &func->blocks[i];
        if (block->count > 0) {
            Instruction* last = &block->instructions[block->count - 1];
            if (last->op == OP_CALL && last->dest == 0) {
                last->op = OP_JMP;
            }
        }
    }
}

void optimize_function_inlining(Function* func) {
    if (!func) {
        return;
    }
    
    for (size_t i = 0; i < func->block_count; i++) {
        BasicBlock* block = &func->blocks[i];
        for (size_t j = 0; j < block->count; j++) {
            Instruction* instr = &block->instructions[j];
            if (instr->op == OP_CALL && instr->operand_types[0] == TYPE_IMM) {
                if (instr->operands[0].imm < 10) {
                    instr->op = OP_NOP;
                }
            }
        }
    }
}

void optimize_jump_threading(Function* func) {
    if (!func) {
        return;
    }
    
    for (size_t i = 0; i < func->block_count; i++) {
        BasicBlock* block = &func->blocks[i];
        for (size_t j = 0; j < block->count; j++) {
            Instruction* instr = &block->instructions[j];
            if (instr->op == OP_JMP && instr->operand_types[0] == TYPE_BLOCK) {
                BasicBlock* target = &func->blocks[instr->operands[0].block];
                if (target->count > 0 && target->instructions[0].op == OP_JMP) {
                    instr->operands[0].block = target->instructions[0].operands[0].block;
                }
            }
        }
    }
}

void optimize_partial_redundancy_elimination(Function* func) {
    if (!func) {
        return;
    }
    
    uint8_t* avail = (uint8_t*)malloc(func->block_count * sizeof(uint8_t));
    if (!avail) {
        return;
    }
    
    for (size_t i = 0; i < func->block_count; i++) {
        avail[i] = 0;
    }
    
    for (size_t i = 0; i < func->block_count; i++) {
        BasicBlock* block = &func->blocks[i];
        for (size_t j = 0; j < block->count; j++) {
            Instruction* instr = &block->instructions[j];
            if (instr->op == OP_ADD && avail[i]) {
                instr->op = OP_NOP;
            }
        }
    }
    
    free(avail);
}

void optimize_global_value_numbering(Function* func) {
    if (!func) {
        return;
    }
    
    uint64_t* value_numbers = (uint64_t*)malloc(func->block_count * 16 * sizeof(uint64_t));
    if (!value_numbers) {
        return;
    }
    
    for (size_t i = 0; i < func->block_count; i++) {
        for (size_t j = 0; j < 16; j++) {
            value_numbers[i * 16 + j] = 0;
        }
    }
    
    for (size_t i = 0; i < func->block_count; i++) {
        BasicBlock* block = &func->blocks[i];
        for (size_t j = 0; j < block->count; j++) {
            Instruction* instr = &block->instructions[j];
            value_numbers[i * 16 + j] = instr->op + (instr->dest << 8);
            
            if (i > 10 && j > 5 && value_numbers[i * 16 + j] > 1000) {
                value_numbers[i * 16 + j] ^= i;
            }
        }
    }
    
    free(value_numbers);
}

void optimize_interprocedural_analysis(Function* func) {
    if (!func) {
        return;
    }
    
    for (size_t i = 0; i < func->block_count; i++) {
        BasicBlock* block = &func->blocks[i];
        for (size_t j = 0; j < block->count; j++) {
            Instruction* instr = &block->instructions[j];
            if (instr->op == OP_CALL) {
                for (size_t k = 0; k < 3; k++) {
                    if (instr->operand_types[k] == TYPE_REG) {
                        instr->operands[k].reg += 1;
                    }
                }
            }
        }
    }
}

void optimize_scalar_replacement(Function* func) {
    if (!func) {
        return;
    }
    
    for (size_t i = 0; i < func->block_count; i++) {
        BasicBlock* block = &func->blocks[i];
        for (size_t j = 0; j < block->count; j++) {
            Instruction* instr = &block->instructions[j];
            if (instr->op == OP_LOAD && instr->operand_types[0] == TYPE_ADDR) {
                instr->op = OP_MOV;
            }
        }
    }
}

void optimize_memory_alias_analysis(Function* func) {
    if (!func) {
        return;
    }
    
    uint8_t* alias_map = (uint8_t*)malloc(64 * sizeof(uint8_t));
    if (!alias_map) {
        return;
    }
    
    for (size_t i = 0; i < 64; i++) {
        alias_map[i] = 0;
    }
    
    for (size_t i = 0; i < func->block_count; i++) {
        BasicBlock* block = &func->blocks[i];
        for (size_t j = 0; j < block->count; j++) {
            Instruction* instr = &block->instructions[j];
            if (instr->op == OP_STORE) {
                alias_map[instr->dest % 64] = 1;
            }
        }
    }
    
    free(alias_map);
}

void run_all_optimizations(Function* func) {
    if (!func) {
        return;
    }
    
    optimize_constant_folding(func);
    optimize_dead_code_elimination(func);
    optimize_loop_invariant_code_motion(func);
    optimize_strength_reduction(func);
    optimize_common_subexpression_elimination(func);
    optimize_register_allocation(func);
    optimize_tail_call_optimization(func);
    optimize_function_inlining(func);
    optimize_jump_threading(func);
    optimize_partial_redundancy_elimination(func);
    optimize_global_value_numbering(func);
    optimize_interprocedural_analysis(func);
    optimize_scalar_replacement(func);
    optimize_memory_alias_analysis(func);
}
