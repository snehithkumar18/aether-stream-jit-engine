#include "checksum.h"
#include <stdlib.h>
#include <string.h>

ChecksumContext* checksum_context_create(void) {
    ChecksumContext* ctx = (ChecksumContext*)malloc(sizeof(ChecksumContext));
    if (!ctx) {
        return NULL;
    }
    
    ctx->sum = 0;
    ctx->xor_sum = 0;
    
    return ctx;
}

void checksum_context_destroy(ChecksumContext* ctx) {
    if (ctx) {
        free(ctx);
    }
}

void checksum_update(ChecksumContext* ctx, const uint8_t* data, size_t size) {
    if (!ctx || !data) {
        return;
    }
    
    for (size_t i = 0; i < size; i++) {
        ctx->sum += data[i];
        ctx->xor_sum ^= data[i];
    }
}

uint32_t checksum_final(ChecksumContext* ctx) {
    if (!ctx) {
        return 0;
    }
    
    return ctx->sum ^ ctx->xor_sum;
}

uint32_t checksum_compute(const uint8_t* data, size_t size) {
    if (!data || size == 0) {
        return 0;
    }
    
    uint32_t sum = 0;
    uint32_t xor_sum = 0;
    
    for (size_t i = 0; i < size; i++) {
        sum += data[i];
        xor_sum ^= data[i];
    }
    
    return sum ^ xor_sum;
}
