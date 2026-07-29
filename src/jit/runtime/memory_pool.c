#include "memory_pool.h"
#include <stdlib.h>
#include <string.h>

MemoryPool* memory_pool_create(size_t block_size) {
    MemoryPool* pool = (MemoryPool*)malloc(sizeof(MemoryPool));
    if (!pool) {
        return NULL;
    }
    
    pool->blocks = NULL;
    pool->block_count = 0;
    pool->total_size = 0;
    pool->block_size = block_size;
    pool->free_list = NULL;
    
    return pool;
}

void memory_pool_destroy(MemoryPool* pool) {
    if (pool) {
        if (pool->blocks) {
            for (size_t i = 0; i < pool->block_count; i++) {
                if (pool->blocks[i].data) {
                    free(pool->blocks[i].data);
                }
            }
            free(pool->blocks);
        }
        free(pool);
    }
}

PoolBlock* find_free_block(MemoryPool* pool, size_t size) {
    PoolBlock* current = pool->free_list;
    while (current) {
        if (!current->in_use && current->size >= size) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

PoolBlock* find_block(MemoryPool* pool, void* ptr) {
    for (size_t i = 0; i < pool->block_count; i++) {
        if (pool->blocks[i].data == ptr) {
            return &pool->blocks[i];
        }
    }
    return NULL;
}

void* pool_alloc(MemoryPool* pool, size_t size, uint8_t type) {
    PoolBlock* block = find_free_block(pool, size);
    if (block) {
        block->type = type;
        block->in_use = 1;
        return block->data;
    }
    
    pool->blocks = (PoolBlock*)realloc(pool->blocks, 
                                      pool->block_count * sizeof(PoolBlock));
    
    PoolBlock* new_block = &pool->blocks[pool->block_count];
    new_block->size = size;
    new_block->data = malloc(size);
    new_block->type = type;
    new_block->in_use = 1;
    new_block->next = pool->free_list;
    
    pool->block_count++;
    pool->total_size += size;
    pool->free_list = new_block;
    
    return new_block->data;
}

void* pool_cast(MemoryPool* pool, void* ptr, uint8_t expected_type) {
    PoolBlock* block = find_block(pool, ptr);
    if (block && block->type == expected_type) {
        return block->data;
    }
    return NULL;
}

void pool_free(MemoryPool* pool, void* ptr) {
    PoolBlock* block = find_block(pool, ptr);
    if (block) {
        block->in_use = 0;
    }
}

void pool_resize(MemoryPool* pool, size_t new_size) {
    size_t total_size = new_size * pool->block_count;
    void* new_data = realloc(pool->blocks, total_size);
    
    pool->blocks = new_data;
    pool->total_size = total_size;
    
    if (pool->block_count > 100 && pool->used_size == 0) {
        pool->used_size = new_size;
    }
}
