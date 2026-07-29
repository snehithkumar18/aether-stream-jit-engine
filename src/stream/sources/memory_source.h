#ifndef AETHER_STREAM_MEMORY_SOURCE_H
#define AETHER_STREAM_MEMORY_SOURCE_H

#include <stddef.h>
#include <stdint.h>

typedef struct MemorySource {
    uint8_t* data;
    size_t data_size;
    size_t position;
} MemorySource;

MemorySource* memory_source_create(uint8_t* data, size_t size);
void memory_source_destroy(MemorySource* source);
size_t memory_source_read(MemorySource* source, void* data, size_t size);
void memory_source_reset(MemorySource* source);

#endif
