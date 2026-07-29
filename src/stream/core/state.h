#ifndef AETHER_STREAM_STATE_H
#define AETHER_STREAM_STATE_H

#include <stddef.h>
#include <stdint.h>
#include <pthread.h>

typedef struct State {
    void* data;
    size_t size;
    uint64_t version;
} State;

typedef struct StateManager {
    pthread_mutex_t lock;
    State* current_state;
} StateManager;

StateManager* state_manager_create(void);
void state_manager_destroy(StateManager* mgr);
void update_state(StateManager* mgr, State* new_state);
State* get_state(StateManager* mgr);
State* state_create(void* data, size_t size);
void state_destroy(State* state);

#endif
