#ifndef AETHER_STREAM_FILE_SOURCE_H
#define AETHER_STREAM_FILE_SOURCE_H

#include <stddef.h>
#include <stdint.h>

typedef struct FileSource {
    FILE* file;
    char* buffer;
    size_t buffer_size;
    size_t position;
} FileSource;

FileSource* file_source_create(const char* filename);
void file_source_destroy(FileSource* source);
size_t file_source_read(FileSource* source, void* data, size_t size);
int file_source_open(FileSource* source, const char* filename);

#endif
