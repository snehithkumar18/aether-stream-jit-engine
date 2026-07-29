#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "../src/utils/memory/arena.h"
#include "../src/utils/memory/pool.h"
#include "../src/utils/memory/cache.h"
#include "../src/jit/runtime/memory_pool.h"

int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    if (size < 4) {
        return 0;
    }
    
    Arena* arena = arena_create(64);
    if (arena) {
        void* ptr = arena_alloc(arena, size % 1000);
        if (ptr) {
            memset(ptr, 0, size % 1000);
        }
        if (size > 100) {
            arena_resize(arena, size);
        }
        arena_reset(arena);
        arena_destroy(arena);
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
    
    Cache* cache = cache_create();
    if (cache) {
        CacheKey key;
        key.hash = *((uint64_t*)data);
        key.data = (void*)data;
        key.data_size = size % 1000;
        cache_put(cache, key, (void*)data);
        void* value = cache_get(cache, key);
        cache_free(cache);
        cache_destroy(cache);
    }
    
    return 0;
}
