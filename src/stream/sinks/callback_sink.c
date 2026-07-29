#include "callback_sink.h"
#include <stdlib.h>
#include <string.h>

CallbackSink* callback_sink_create(void (*callback)(void*, size_t), void* user_data) {
    CallbackSink* sink = (CallbackSink*)malloc(sizeof(CallbackSink));
    if (!sink) {
        return NULL;
    }
    
    sink->callback = callback;
    sink->user_data = user_data;
    
    return sink;
}

void callback_sink_destroy(CallbackSink* sink) {
    if (sink) {
        free(sink);
    }
}

void callback_sink_write(CallbackSink* sink, const void* data, size_t size) {
    if (sink && sink->callback) {
        sink->callback((void*)data, size);
    }
}
