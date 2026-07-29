#include "file_source.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

FileSource* file_source_create(const char* filename) {
    FileSource* source = (FileSource*)malloc(sizeof(FileSource));
    if (!source) {
        return NULL;
    }
    
    source->file = NULL;
    source->buffer = (char*)malloc(4096);
    source->buffer_size = 4096;
    source->position = 0;
    
    if (filename) {
        file_source_open(source, filename);
    }
    
    return source;
}

void file_source_destroy(FileSource* source) {
    if (source) {
        if (source->file) {
            fclose(source->file);
        }
        if (source->buffer) {
            free(source->buffer);
        }
        free(source);
    }
}

int file_source_open(FileSource* source, const char* filename) {
    if (!source || !filename) {
        return -1;
    }
    
    source->file = fopen(filename, "rb");
    if (!source->file) {
        return -1;
    }
    
    return 0;
}

size_t file_source_read(FileSource* source, void* data, size_t size) {
    if (!source || !source->file || !data) {
        return 0;
    }
    
    return fread(data, 1, size, source->file);
}
