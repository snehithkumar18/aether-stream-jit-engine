#ifndef AETHER_UTILS_CRYPTO_HASH_H
#define AETHER_UTILS_CRYPTO_HASH_H

#include <stddef.h>
#include <stdint.h>

typedef struct HashContext {
    uint32_t state[8];
    uint64_t total_size;
} HashContext;

HashContext* hash_context_create(void);
void hash_context_destroy(HashContext* ctx);
void hash_update(HashContext* ctx, const uint8_t* data, size_t size);
void hash_final(HashContext* ctx, uint8_t* out_hash);
uint64_t hash_compute(const uint8_t* data, size_t size);

#endif
