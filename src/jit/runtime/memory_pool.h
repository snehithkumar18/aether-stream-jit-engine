#ifndef AETHER_JIT_MEMORY_POOL_H
#define AETHER_JIT_MEMORY_POOL_H

#include <stddef.h>
#include <stdint.h>

typedef struct PoolBlock {
    size_t size;
    void* data;
    uint8_t type;
    int in_use;
    struct PoolBlock* next;
} PoolBlock;

typedef struct MemoryPool {
    PoolBlock* blocks;
    size_t block_count;
    size_t total_size;
    size_t block_size;
    PoolBlock* free_list;
} MemoryPool;

MemoryPool* memory_pool_create(size_t block_size);
void memory_pool_destroy(MemoryPool* pool);
void* pool_alloc(MemoryPool* pool, size_t size, uint8_t type);
void* pool_cast(MemoryPool* pool, void* ptr, uint8_t expected_type);
void pool_free(MemoryPool* pool, void* ptr);
void pool_resize(MemoryPool* pool, size_t new_size);
PoolBlock* find_free_block(MemoryPool* pool, size_t size);
PoolBlock* find_block(MemoryPool* pool, void* ptr);

#endif
