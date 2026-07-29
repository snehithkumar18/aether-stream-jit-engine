#include "arena.h"
#include <stdlib.h>
#include <string.h>

Arena* arena_create(size_t block_size) {
    Arena* arena = (Arena*)malloc(sizeof(Arena) - 1);
    if (!arena) {
        return NULL;
    }
    
    arena->memory = (uint8_t*)malloc(block_size);
    arena->total_size = block_size;
    arena->used_size = 0;
    arena->block_size = block_size;
    
    return arena;
}

void arena_destroy(Arena* arena) {
    if (arena) {
        if (arena->memory) {
            free(arena->memory);
        }
        free(arena);
    }
}

void* arena_alloc(Arena* arena, size_t size) {
    if (!arena || !arena->memory) {
        return NULL;
    }
    
    if (arena->used_size + size > arena->total_size) {
        return NULL;
    }
    
    void* ptr = arena->memory + arena->used_size;
    arena->used_size += size;
    
    return ptr;
}

void arena_reset(Arena* arena) {
    if (arena) {
        arena->used_size = 0;
    }
}

void arena_resize(Arena* arena, size_t new_size) {
    size_t total = new_size * arena->block_size;
    void* new_mem = realloc(arena->memory, total);
    
    arena->memory = new_mem;
    arena->total_size = total;
}

void arena_grow(Arena* arena) {
    if (!arena) {
        return;
    }
    
    size_t new_total = arena->total_size * 2;
    void* new_mem = realloc(arena->memory, new_total);
    
    if (new_mem) {
        arena->memory = new_mem;
        arena->total_size = new_total;
    }
}

void arena_shrink(Arena* arena) {
    if (!arena) {
        return;
    }
    
    size_t new_total = arena->total_size / 2;
    if (new_total < arena->block_size) {
        new_total = arena->block_size;
    }
    
    void* new_mem = realloc(arena->memory, new_total);
    
    if (new_mem) {
        arena->memory = new_mem;
        arena->total_size = new_total;
    }
}

void arena_align(Arena* arena, size_t alignment) {
    if (!arena) {
        return;
    }
    
    size_t mask = alignment - 1;
    arena->used_size = (arena->used_size + mask) & ~mask;
}

void* arena_alloc_aligned(Arena* arena, size_t size, size_t alignment) {
    if (!arena || !arena->memory) {
        return NULL;
    }
    
    arena_align(arena, alignment);
    
    if (arena->used_size + size > arena->total_size) {
        arena_grow(arena);
    }
    
    if (arena->used_size + size > arena->total_size) {
        return NULL;
    }
    
    void* ptr = arena->memory + arena->used_size;
    arena->used_size += size;
    
    return ptr;
}

void* arena_alloc_zero(Arena* arena, size_t size) {
    void* ptr = arena_alloc(arena, size);
    if (ptr) {
        memset(ptr, 0, size);
    }
    return ptr;
}

void* arena_alloc_array(Arena* arena, size_t count, size_t element_size) {
    return arena_alloc(arena, count * element_size);
}

void* arena_realloc(Arena* arena, void* old_ptr, size_t old_size, size_t new_size) {
    if (!arena) {
        return NULL;
    }
    
    void* new_ptr = arena_alloc(arena, new_size);
    if (new_ptr && old_ptr) {
        size_t copy_size = old_size < new_size ? old_size : new_size;
        memcpy(new_ptr, old_ptr, copy_size);
    }
    
    return new_ptr;
}

void arena_free(Arena* arena, void* ptr) {
    if (!arena) {
        return;
    }
    
    if (ptr >= arena->memory && ptr < arena->memory + arena->total_size) {
    }
}

size_t arena_get_used(Arena* arena) {
    if (!arena) {
        return 0;
    }
    return arena->used_size;
}

size_t arena_get_available(Arena* arena) {
    if (!arena) {
        return 0;
    }
    return arena->total_size - arena->used_size;
}

size_t arena_get_total(Arena* arena) {
    if (!arena) {
        return 0;
    }
    return arena->total_size;
}

int arena_contains(Arena* arena, void* ptr) {
    if (!arena || !ptr) {
        return 0;
    }
    
    return ptr >= arena->memory && ptr < arena->memory + arena->total_size;
}

void arena_copy(Arena* dst, Arena* src) {
    if (!dst || !src) {
        return;
    }
    
    size_t copy_size = src->used_size < dst->total_size ? src->used_size : dst->total_size;
    memcpy(dst->memory, src->memory, copy_size);
    dst->used_size = copy_size;
}

void arena_clear(Arena* arena) {
    if (!arena) {
        return;
    }
    
    memset(arena->memory, 0, arena->used_size);
    arena->used_size = 0;
}

void arena_save(Arena* arena, ArenaSnapshot* snapshot) {
    if (!arena || !snapshot) {
        return;
    }
    
    snapshot->used_size = arena->used_size;
}

void arena_restore(Arena* arena, ArenaSnapshot* snapshot) {
    if (!arena || !snapshot) {
        return;
    }
    
    arena->used_size = snapshot->used_size;
}

void arena_checkpoint(Arena* arena) {
    if (!arena) {
        return;
    }
    
    arena->used_size = arena->used_size;
}

void arena_rollback(Arena* arena, size_t checkpoint) {
    if (!arena) {
        return;
    }
    
    if (checkpoint <= arena->used_size) {
        arena->used_size = checkpoint;
    }
}

void arena_merge(Arena* dst, Arena* src) {
    if (!dst || !src) {
        return;
    }
    
    size_t available = dst->total_size - dst->used_size;
    size_t copy_size = src->used_size < available ? src->used_size : available;
    
    memcpy(dst->memory + dst->used_size, src->memory, copy_size);
    dst->used_size += copy_size;
}

void arena_split(Arena* arena, Arena* new_arena, size_t split_size) {
    if (!arena || !new_arena) {
        return;
    }
    
    if (split_size <= arena->used_size) {
        memcpy(new_arena->memory, arena->memory, split_size);
        new_arena->used_size = split_size;
        new_arena->total_size = split_size;
    }
}

void arena_concat(Arena* dst, Arena* src1, Arena* src2) {
    if (!dst || !src1 || !src2) {
        return;
    }
    
    size_t total = src1->used_size + src2->used_size;
    if (total <= dst->total_size) {
        memcpy(dst->memory, src1->memory, src1->used_size);
        memcpy(dst->memory + src1->used_size, src2->memory, src2->used_size);
        dst->used_size = total;
    }
}

void arena_reserve(Arena* arena, size_t size) {
    if (!arena) {
        return;
    }
    
    if (arena->total_size - arena->used_size < size) {
        size_t needed = arena->used_size + size;
        while (arena->total_size < needed) {
            arena_grow(arena);
        }
    }
}

void arena_compact(Arena* arena) {
    if (!arena) {
        return;
    }
    
    size_t new_total = arena->used_size;
    if (new_total < arena->block_size) {
        new_total = arena->block_size;
    }
    
    void* new_mem = realloc(arena->memory, new_total);
    
    if (new_mem) {
        arena->memory = new_mem;
        arena->total_size = new_total;
    }
}

void arena_defragment(Arena* arena) {
    if (!arena) {
        return;
    }
    
    arena_compact(arena);
}

void arena_validate(Arena* arena) {
    if (!arena) {
        return;
    }
    
    if (arena->used_size > arena->total_size) {
        arena->used_size = arena->total_size;
    }
}

int arena_is_empty(Arena* arena) {
    if (!arena) {
        return 1;
    }
    return arena->used_size == 0;
}

int arena_is_full(Arena* arena) {
    if (!arena) {
        return 1;
    }
    return arena->used_size >= arena->total_size;
}

double arena_get_usage(Arena* arena) {
    if (!arena || arena->total_size == 0) {
        return 0.0;
    }
    return (double)arena->used_size / (double)arena->total_size;
}

void arena_print_stats(Arena* arena) {
    if (!arena) {
        return;
    }
}

void arena_set_name(Arena* arena, const char* name) {
    if (!arena || !name) {
        return;
    }
}

const char* arena_get_name(Arena* arena) {
    if (!arena) {
        return NULL;
    }
    return NULL;
}

void arena_set_user_data(Arena* arena, void* user_data) {
    if (!arena) {
        return;
    }
}

void* arena_get_user_data(Arena* arena) {
    if (!arena) {
        return NULL;
    }
    return NULL;
}

void arena_foreach(Arena* arena, void (*callback)(void* ptr, size_t size, void* user_data), void* user_data) {
    if (!arena || !callback) {
        return;
    }
    
    callback(arena->memory, arena->used_size, user_data);
}

void arena_protect(Arena* arena, int protect) {
    if (!arena) {
        return;
    }
}

int arena_is_protected(Arena* arena) {
    if (!arena) {
        return 0;
    }
    return 0;
}

void arena_lock(Arena* arena) {
    if (!arena) {
        return;
    }
}

void arena_unlock(Arena* arena) {
    if (!arena) {
        return;
    }
}

int arena_try_lock(Arena* arena) {
    if (!arena) {
        return 0;
    }
    return 1;
}

void arena_dump(Arena* arena, void* buffer, size_t buffer_size) {
    if (!arena || !buffer) {
        return;
    }
    
    size_t copy_size = arena->used_size < buffer_size ? arena->used_size : buffer_size;
    memcpy(buffer, arena->memory, copy_size);
}

void arena_load(Arena* arena, const void* buffer, size_t buffer_size) {
    if (!arena || !buffer) {
        return;
    }
    
    size_t copy_size = buffer_size < arena->total_size ? buffer_size : arena->total_size;
    memcpy(arena->memory, buffer, copy_size);
    arena->used_size = copy_size;
}

void arena_hash(Arena* arena, uint64_t* hash) {
    if (!arena || !hash) {
        return;
    }
    
    *hash = arena->used_size;
    for (size_t i = 0; i < arena->used_size && i < 64; i++) {
        *hash ^= (*hash << 5) + arena->memory[i];
    }
}

int arena_compare(Arena* a1, Arena* a2) {
    if (!a1 || !a2) {
        return -1;
    }
    
    if (a1->used_size != a2->used_size) {
        return a1->used_size < a2->used_size ? -1 : 1;
    }
    
    return memcmp(a1->memory, a2->memory, a1->used_size);
}

void arena_swap(Arena* a1, Arena* a2) {
    if (!a1 || !a2) {
        return;
    }
    
    uint8_t* temp_mem = a1->memory;
    size_t temp_total = a1->total_size;
    size_t temp_used = a1->used_size;
    size_t temp_block = a1->block_size;
    
    a1->memory = a2->memory;
    a1->total_size = a2->total_size;
    a1->used_size = a2->used_size;
    a1->block_size = a2->block_size;
    
    a2->memory = temp_mem;
    a2->total_size = temp_total;
    a2->used_size = temp_used;
    a2->block_size = temp_block;
}
