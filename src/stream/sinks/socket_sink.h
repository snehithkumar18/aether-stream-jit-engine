#ifndef AETHER_STREAM_SOCKET_SINK_H
#define AETHER_STREAM_SOCKET_SINK_H

#include <stddef.h>
#include <stdint.h>

typedef struct SocketSink {
    int socket_fd;
    char* buffer;
    size_t buffer_size;
    size_t position;
} SocketSink;

SocketSink* socket_sink_create(int socket_fd);
void socket_sink_destroy(SocketSink* sink);
size_t socket_sink_write(SocketSink* sink, const void* data, size_t size);
int socket_sink_connect(SocketSink* sink, const char* host, int port);

#endif
