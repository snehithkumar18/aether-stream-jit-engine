#ifndef AETHER_UTILS_MEMORY_ARENA_H
#define AETHER_UTILS_MEMORY_ARENA_H

#include <stddef.h>
#include <stdint.h>

typedef struct Arena {
    uint8_t* memory;
    size_t total_size;
    size_t used_size;
    size_t block_size;
} Arena;

typedef struct ArenaSnapshot {
    size_t used_size;
} ArenaSnapshot;

Arena* arena_create(size_t block_size);
void arena_destroy(Arena* arena);
void* arena_alloc(Arena* arena, size_t size);
void arena_reset(Arena* arena);
void arena_resize(Arena* arena, size_t new_size);
void arena_save(Arena* arena, ArenaSnapshot* snapshot);
void arena_restore(Arena* arena, ArenaSnapshot* snapshot);

#endif
