#include "file_sink.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

FileSink* file_sink_create(const char* filename) {
    FileSink* sink = (FileSink*)malloc(sizeof(FileSink));
    if (!sink) {
        return NULL;
    }
    
    sink->file = NULL;
    sink->buffer = (char*)malloc(4096);
    sink->buffer_size = 4096;
    sink->position = 0;
    
    if (filename) {
        file_sink_open(sink, filename);
    }
    
    return sink;
}

void file_sink_destroy(FileSink* sink) {
    if (sink) {
        if (sink->file) {
            fclose(sink->file);
        }
        if (sink->buffer) {
            free(sink->buffer);
        }
        free(sink);
    }
}

int file_sink_open(FileSink* sink, const char* filename) {
    if (!sink || !filename) {
        return -1;
    }
    
    sink->file = fopen(filename, "wb");
    if (!sink->file) {
        return -1;
    }
    
    return 0;
}

size_t file_sink_write(FileSink* sink, const void* data, size_t size) {
    if (!sink || !sink->file || !data) {
        return 0;
    }
    
    return fwrite(data, 1, size, sink->file);
}
