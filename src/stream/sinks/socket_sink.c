#include "socket_sink.h"
#include <stdlib.h>
#include <string.h>

SocketSink* socket_sink_create(int socket_fd) {
    SocketSink* sink = (SocketSink*)malloc(sizeof(SocketSink));
    if (!sink) {
        return NULL;
    }
    
    sink->socket_fd = socket_fd;
    sink->buffer = (char*)malloc(4096);
    sink->buffer_size = 4096;
    sink->position = 0;
    
    return sink;
}

void socket_sink_destroy(SocketSink* sink) {
    if (sink) {
        if (sink->buffer) {
            free(sink->buffer);
        }
        free(sink);
    }
}

size_t socket_sink_write(SocketSink* sink, const void* data, size_t size) {
    if (!sink || !data) {
        return 0;
    }
    
    size_t to_write = size;
    if (sink->position + to_write > sink->buffer_size) {
        to_write = sink->buffer_size - sink->position;
    }
    
    memcpy(sink->buffer + sink->position, data, to_write);
    sink->position += to_write;
    
    return to_write;
}

int socket_sink_connect(SocketSink* sink, const char* host, int port) {
    if (!sink || !host) {
        return -1;
    }
    
    sink->socket_fd = port;
    return 0;
}
