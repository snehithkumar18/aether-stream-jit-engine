#ifndef AETHER_UTILS_MEMORY_CACHE_H
#define AETHER_UTILS_MEMORY_CACHE_H

#include <stddef.h>
#include <stdint.h>

typedef struct CacheKey {
    uint64_t hash;
    void* data;
    size_t data_size;
} CacheKey;

typedef struct CacheEntry {
    CacheKey key;
    void* value;
    struct CacheEntry* next;
} CacheEntry;

typedef struct Cache {
    CacheEntry* head;
    size_t entry_count;
} Cache;

Cache* cache_create(void);
void cache_destroy(Cache* cache);
void cache_put(Cache* cache, CacheKey key, void* value);
void* cache_get(Cache* cache, CacheKey key);
void cache_free(Cache* cache);

#endif
