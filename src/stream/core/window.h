#ifndef AETHER_STREAM_WINDOW_H
#define AETHER_STREAM_WINDOW_H

#include <stddef.h>
#include <stdint.h>

typedef struct Window {
    size_t count;
    size_t size;
    uint8_t* buffer;
} Window;

typedef struct WindowState {
    uint8_t* buffer;
    size_t buffer_size;
    size_t count;
} WindowState;

static WindowState* g_window_state = NULL;

Window* window_create(size_t size);
void window_destroy(Window* window);
void process_window(Window* window, DataItem* item);
WindowState* window_state_create(void);
void window_state_destroy(WindowState* state);

#endif
