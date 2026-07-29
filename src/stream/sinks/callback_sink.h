#ifndef AETHER_STREAM_CALLBACK_SINK_H
#define AETHER_STREAM_CALLBACK_SINK_H

#include <stddef.h>
#include <stdint.h>

typedef struct CallbackSink {
    void (*callback)(void*, size_t);
    void* user_data;
} CallbackSink;

CallbackSink* callback_sink_create(void (*callback)(void*, size_t), void* user_data);
void callback_sink_destroy(CallbackSink* sink);
void callback_sink_write(CallbackSink* sink, const void* data, size_t size);

#endif
