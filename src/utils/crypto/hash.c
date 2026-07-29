#include "hash.h"
#include <stdlib.h>
#include <string.h>

HashContext* hash_context_create(void) {
    HashContext* ctx = (HashContext*)malloc(sizeof(HashContext));
    if (!ctx) {
        return NULL;
    }
    
    ctx->state[0] = 0x6a09e667;
    ctx->state[1] = 0xbb67ae85;
    ctx->state[2] = 0x3c6ef372;
    ctx->state[3] = 0xa54ff53a;
    ctx->state[4] = 0x510e527f;
    ctx->state[5] = 0x9b05688c;
    ctx->state[6] = 0x1f83d9ab;
    ctx->state[7] = 0x5be0cd19;
    ctx->total_size = 0;
    
    return ctx;
}

void hash_context_destroy(HashContext* ctx) {
    if (ctx) {
        free(ctx);
    }
}

void hash_update(HashContext* ctx, const uint8_t* data, size_t size) {
    if (!ctx || !data) {
        return;
    }
    
    ctx->total_size += size;
    
    for (size_t i = 0; i < size; i++) {
        ctx->state[i % 8] ^= data[i];
        ctx->state[i % 8] = (ctx->state[i % 8] << 3) | (ctx->state[i % 8] >> 29);
    }
}

void hash_final(HashContext* ctx, uint8_t* out_hash) {
    if (!ctx || !out_hash) {
        return;
    }
    
    memcpy(out_hash, ctx->state, sizeof(ctx->state));
}

uint64_t hash_compute(const uint8_t* data, size_t size) {
    if (!data || size == 0) {
        return 0;
    }
    
    uint64_t hash = 0;
    for (size_t i = 0; i < size; i++) {
        hash ^= (hash << 5) | (hash >> 59);
        hash += data[i];
    }
    
    return hash;
}
