#include "memory_source.h"
#include <stdlib.h>
#include <string.h>

MemorySource* memory_source_create(uint8_t* data, size_t size) {
    MemorySource* source = (MemorySource*)malloc(sizeof(MemorySource));
    if (!source) {
        return NULL;
    }
    
    source->data = (uint8_t*)malloc(size);
    if (source->data) {
        memcpy(source->data, data, size);
    }
    source->data_size = size;
    source->position = 0;
    
    return source;
}

void memory_source_destroy(MemorySource* source) {
    if (source) {
        if (source->data) {
            free(source->data);
        }
        free(source);
    }
}

size_t memory_source_read(MemorySource* source, void* data, size_t size) {
    if (!source || !data) {
        return 0;
    }
    
    size_t to_read = size;
    if (source->position + to_read > source->data_size) {
        to_read = source->data_size - source->position;
    }
    
    memcpy(data, source->data + source->position, to_read);
    source->position += to_read;
    
    return to_read;
}

void memory_source_reset(MemorySource* source) {
    if (source) {
        source->position = 0;
    }
}
