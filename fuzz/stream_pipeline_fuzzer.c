#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "../src/stream/core/pipeline.h"
#include "../src/stream/core/operator.h"
#include "../src/stream/core/window.h"
#include "../src/stream/core/state.h"
#include "../src/stream/sources/socket_source.h"
#include "../src/stream/sources/file_source.h"
#include "../src/stream/sources/memory_source.h"
#include "../src/stream/sinks/socket_sink.h"
#include "../src/stream/sinks/file_sink.h"
#include "../src/stream/sinks/callback_sink.h"

int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    if (size < 4) {
        return 0;
    }
    
    Pipeline* pipeline = pipeline_create();
    if (pipeline) {
        Operator* op = operator_create(OP_FILTER, TYPE_INT);
        if (op) {
            op->callback = filter_callback;
            process_operator(pipeline, op);
            operator_destroy(op);
        }
        execute_pipeline(pipeline);
        if (size > 10) {
            reorder_operators(pipeline);
        }
        pipeline_destroy(pipeline);
    }
    
    Operator* op2 = operator_create(OP_MAP, *((DataType*)data));
    if (op2) {
        void* op_data = get_operator_data(op2, TYPE_INT);
        set_operator_data(op2, (void*)data, TYPE_FLOAT);
        operator_destroy(op2);
    }
    
    Window* window = window_create(size % 1000);
    if (window) {
        DataItem item;
        item.type = TYPE_INT;
        item.value = *((int64_t*)data);
        item.data = (void*)data;
        if (window->count > 5) {
            process_window(window, &item);
        }
        window_destroy(window);
    }
    
    StateManager* state_mgr = state_manager_create();
    if (state_mgr) {
        State* state = state_create((void*)data, size % 1000);
        if (state) {
            update_state(state_mgr, state);
            State* current = get_state(state_mgr);
            if (current) {
                state_destroy(current);
            }
        }
        state_manager_destroy(state_mgr);
    }
    
    SocketSource* sock_src = socket_source_create(*((int*)data));
    if (sock_src) {
        socket_source_connect(sock_src, "localhost", 8080);
        uint8_t buffer[100];
        socket_source_read(sock_src, buffer, size % 100);
        socket_source_destroy(sock_src);
    }
    
    FileSource* file_src = file_source_create(NULL);
    if (file_src) {
        uint8_t buffer[100];
        file_source_read(file_src, buffer, size % 100);
        file_source_destroy(file_src);
    }
    
    MemorySource* mem_src = memory_source_create((uint8_t*)data, size);
    if (mem_src) {
        uint8_t buffer[100];
        memory_source_read(mem_src, buffer, size % 100);
        memory_source_destroy(mem_src);
    }
    
    SocketSink* sock_sink = socket_sink_create(*((int*)data));
    if (sock_sink) {
        socket_sink_connect(sock_sink, "localhost", 8080);
        socket_sink_write(sock_sink, data, size % 100);
        socket_sink_destroy(sock_sink);
    }
    
    FileSink* file_sink = file_sink_create(NULL);
    if (file_sink) {
        file_sink_write(file_sink, data, size % 100);
        file_sink_destroy(file_sink);
    }
    
    CallbackSink* cb_sink = callback_sink_create(NULL, NULL);
    if (cb_sink) {
        callback_sink_write(cb_sink, data, size % 100);
        callback_sink_destroy(cb_sink);
    }
    
    return 0;
}
