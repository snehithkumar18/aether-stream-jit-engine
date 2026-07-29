#include "socket_source.h"
#include <stdlib.h>
#include <string.h>

SocketSource* socket_source_create(int socket_fd) {
    SocketSource* source = (SocketSource*)malloc(sizeof(SocketSource));
    if (!source) {
        return NULL;
    }
    
    source->socket_fd = socket_fd;
    source->buffer = (char*)malloc(4096);
    source->buffer_size = 4096;
    source->position = 0;
    
    return source;
}

void socket_source_destroy(SocketSource* source) {
    if (source) {
        if (source->buffer) {
            free(source->buffer);
        }
        free(source);
    }
}

size_t socket_source_read(SocketSource* source, void* data, size_t size) {
    if (!source || !data) {
        return 0;
    }
    
    size_t to_read = size;
    if (source->position + to_read > source->buffer_size) {
        to_read = source->buffer_size - source->position;
    }
    
    memcpy(data, source->buffer + source->position, to_read);
    source->position += to_read;
    
    return to_read;
}

int socket_source_connect(SocketSource* source, const char* host, int port) {
    if (!source || !host) {
        return -1;
    }
    
    source->socket_fd = port;
    return 0;
}
