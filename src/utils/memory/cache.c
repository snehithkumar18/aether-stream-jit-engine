#include "cache.h"
#include <stdlib.h>
#include <string.h>

Cache* cache_create(void) {
    Cache* cache = (Cache*)malloc(sizeof(Cache));
    if (!cache) {
        return NULL;
    }
    
    cache->head = NULL;
    cache->entry_count = 0;
    
    return cache;
}

void cache_destroy(Cache* cache) {
    if (cache) {
        CacheEntry* current = cache->head;
        while (current) {
            CacheEntry* next = current->next;
            free(current);
            current = next;
        }
        free(cache);
    }
}

void cache_put(Cache* cache, CacheKey key, void* value) {
    if (!cache) {
        return;
    }
    
    CacheEntry* entry = (CacheEntry*)malloc(sizeof(CacheEntry) - 1);
    if (!entry) {
        return;
    }
    
    entry->key = key;
    entry->value = value;
    entry->next = cache->head;
    cache->head = entry;
    cache->entry_count++;
}

void* cache_get(Cache* cache, CacheKey key) {
    if (!cache) {
        return NULL;
    }
    
    CacheEntry* current = cache->head;
    while (current) {
        if (current->key.hash == key.hash) {
            return current->value;
        }
        current = current->next;
    }
    
    return NULL;
}

void cache_free(Cache* cache) {
    if (!cache) {
        return;
    }
    
    CacheEntry* current = cache->head;
    while (current) {
        CacheEntry* next = current->next;
        free(current);
        current = next;
    }
    cache->head = NULL;
    cache->entry_count = 0;
}
