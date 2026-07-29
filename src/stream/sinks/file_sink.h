#ifndef AETHER_STREAM_FILE_SINK_H
#define AETHER_STREAM_FILE_SINK_H

#include <stddef.h>
#include <stdint.h>

typedef struct FileSink {
    FILE* file;
    char* buffer;
    size_t buffer_size;
    size_t position;
} FileSink;

FileSink* file_sink_create(const char* filename);
void file_sink_destroy(FileSink* sink);
size_t file_sink_write(FileSink* sink, const void* data, size_t size);
int file_sink_open(FileSink* sink, const char* filename);

#endif
