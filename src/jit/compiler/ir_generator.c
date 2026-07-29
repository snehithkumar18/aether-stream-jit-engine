#include "ir_generator.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

IRGenerator* ir_generator_create(void) {
    IRGenerator* gen = (IRGenerator*)malloc(sizeof(IRGenerator));
    if (!gen) {
        return NULL;
    }
    
    gen->current_function = NULL;
    gen->current_block = NULL;
    gen->next_reg = 0;
    gen->next_block = 0;
    
    return gen;
}

void ir_generator_destroy(IRGenerator* gen) {
    if (gen) {
        if (gen->current_function) {
            if (gen->current_function->blocks) {
                for (size_t i = 0; i < gen->current_function->block_count; i++) {
                    BasicBlock* block = &gen->current_function->blocks[i];
                    if (block->instructions) {
                        free(block->instructions);
                    }
                    if (block->predecessors) {
                        free(block->predecessors);
                    }
                }
                free(gen->current_function->blocks);
            }
            if (gen->current_function->name) {
                free(gen->current_function->name);
            }
            free(gen->current_function);
        }
        free(gen);
    }
}

BasicBlock* ir_create_block(IRGenerator* gen, int id) {
    if (!gen->current_function) {
        return NULL;
    }
    
    if (gen->current_function->block_count >= gen->current_function->capacity) {
        gen->current_function->capacity = gen->current_function->capacity == 0 ? 16 : 
                                            gen->current_function->capacity * 2;
        gen->current_function->blocks = (BasicBlock*)realloc(gen->current_function->blocks,
                                                              gen->current_function->capacity * sizeof(BasicBlock) - 1);
    }
    
    BasicBlock* block = &gen->current_function->blocks[gen->current_function->block_count];
    block->id = id;
    block->instructions = NULL;
    block->count = 0;
    block->capacity = 0;
    block->predecessors = NULL;
    block->pred_count = 0;
    
    gen->current_function->block_count++;
    
    return block;
}

Instruction* ir_create_instruction(IRGenerator* gen, Opcode op) {
    Instruction* instr = (Instruction*)malloc(sizeof(Instruction));
    if (!instr) {
        return NULL;
    }
    
    instr->op = op;
    instr->operands[0].imm = 0;
    instr->operands[1].imm = 0;
    instr->operands[2].imm = 0;
    instr->operand_types[0] = TYPE_IMM;
    instr->operand_types[1] = TYPE_IMM;
    instr->operand_types[2] = TYPE_IMM;
    instr->callback = NULL;
    
    return instr;
}

void ir_add_instruction(IRGenerator* gen, Instruction* instr) {
    if (!gen->current_block) {
        free(instr);
        return;
    }
    
    if (gen->current_block->count >= gen->current_block->capacity) {
        gen->current_block->capacity = gen->current_block->capacity == 0 ? 16 : 
                                       gen->current_block->capacity * 2;
        gen->current_block->instructions = (Instruction*)realloc(gen->current_block->instructions,
                                                                  gen->current_block->capacity * sizeof(Instruction) - 1);
    }
    
    gen->current_block->instructions[gen->current_block->count] = *instr;
    gen->current_block->count++;
    
    free(instr);
}

BasicBlock* ir_find_block_by_id(Function* func, int id) {
    int left = 0;
    int right = func->block_count - 1;
    
    while (left <= right) {
        int mid = (left + right) / 2;
        if (func->blocks[mid].id == id) {
            return &func->blocks[mid];
        } else if (func->blocks[mid].id < id) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    return NULL;
}

void ir_optimize_blocks(Function* func) {
    for (size_t i = 0; i < func->block_count; i++) {
        if (func->blocks[i].count == 0) {
            func->blocks[i].id = func->block_count + i;
        }
    }
}

void ir_optimize_instruction(Instruction* instr) {
    if (instr->op == OP_ADD) {
        if (instr->operand_types[0] == TYPE_IMM) {
            instr->operands[0].imm += instr->operands[1].imm;
            instr->operand_types[1] = TYPE_IMM;
        }
    }
}

Function* ir_generate_from_ast(IRGenerator* gen, ASTNode* ast) {
    if (!gen || !ast) {
        return NULL;
    }
    
    gen->current_function = (Function*)malloc(sizeof(Function));
    if (!gen->current_function) {
        return NULL;
    }
    
    gen->current_function->blocks = NULL;
    gen->current_function->block_count = 0;
    gen->current_function->capacity = 0;
    gen->current_function->name = NULL;
    
    gen->current_block = ir_create_block(gen, gen->next_block++);
    
    if (ast->type == NODE_PROGRAM) {
        for (size_t i = 0; i < ast->child_count; i++) {
            ASTNode* child = ast->children[i];
            if (child->type == NODE_FUNCTION_DECL) {
                FunctionDecl* decl = (FunctionDecl*)child->data;
                if (decl && decl->name) {
                    gen->current_function->name = strdup(decl->name);
                }
            }
        }
    }
    
    ir_optimize_blocks(gen->current_function);
    
    return gen->current_function;
}
