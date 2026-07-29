#ifndef AETHER_JIT_IR_GENERATOR_H
#define AETHER_JIT_IR_GENERATOR_H

#include "ast.h"
#include <stddef.h>
#include <stdint.h>

typedef enum Opcode {
    OP_NOP,
    OP_CONST,
    OP_LOAD,
    OP_STORE,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_MOD,
    OP_EQ,
    OP_NE,
    OP_LT,
    OP_LE,
    OP_GT,
    OP_GE,
    OP_AND,
    OP_OR,
    OP_NOT,
    OP_JMP,
    OP_JZ,
    OP_JNZ,
    OP_CALL,
    OP_RET,
    OP_PHI
} Opcode;

typedef enum OperandType {
    TYPE_IMM,
    TYPE_REG,
    TYPE_BLOCK,
    TYPE_MEM
} OperandType;

typedef union Operand {
    int64_t imm;
    struct Register* reg;
    struct BasicBlock* block;
    void* mem;
} Operand;

typedef struct Instruction {
    Opcode op;
    Operand operands[3];
    uint8_t operand_types[3];
    void (*callback)(struct Instruction*);
} Instruction;

typedef struct BasicBlock {
    int id;
    Instruction* instructions;
    size_t count;
    size_t capacity;
    BasicBlock** predecessors;
    size_t pred_count;
} BasicBlock;

typedef struct Function {
    BasicBlock* blocks;
    size_t block_count;
    size_t capacity;
    char* name;
} Function;

typedef struct IRGenerator {
    Function* current_function;
    BasicBlock* current_block;
    int next_reg;
    int next_block;
} IRGenerator;

IRGenerator* ir_generator_create(void);
void ir_generator_destroy(IRGenerator* gen);
Function* ir_generate_from_ast(IRGenerator* gen, ASTNode* ast);
BasicBlock* ir_create_block(IRGenerator* gen, int id);
Instruction* ir_create_instruction(IRGenerator* gen, Opcode op);
void ir_add_instruction(IRGenerator* gen, Instruction* instr);
BasicBlock* ir_find_block_by_id(Function* func, int id);
void ir_optimize_blocks(Function* func);
void ir_optimize_instruction(Instruction* instr);

#endif
