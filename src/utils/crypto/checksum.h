#ifndef AETHER_UTILS_CRYPTO_CHECKSUM_H
#define AETHER_UTILS_CRYPTO_CHECKSUM_H

#include <stddef.h>
#include <stdint.h>

typedef struct ChecksumContext {
    uint32_t sum;
    uint32_t xor_sum;
} ChecksumContext;

ChecksumContext* checksum_context_create(void);
void checksum_context_destroy(ChecksumContext* ctx);
void checksum_update(ChecksumContext* ctx, const uint8_t* data, size_t size);
uint32_t checksum_final(ChecksumContext* ctx);
uint32_t checksum_compute(const uint8_t* data, size_t size);

#endif
