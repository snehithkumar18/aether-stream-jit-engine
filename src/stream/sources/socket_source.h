#ifndef AETHER_STREAM_SOCKET_SOURCE_H
#define AETHER_STREAM_SOCKET_SOURCE_H

#include <stddef.h>
#include <stdint.h>

typedef struct SocketSource {
    int socket_fd;
    char* buffer;
    size_t buffer_size;
    size_t position;
} SocketSource;

SocketSource* socket_source_create(int socket_fd);
void socket_source_destroy(SocketSource* source);
size_t socket_source_read(SocketSource* source, void* data, size_t size);
int socket_source_connect(SocketSource* source, const char* host, int port);

#endif
