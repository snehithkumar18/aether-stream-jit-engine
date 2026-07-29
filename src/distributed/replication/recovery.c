#include "recovery.h"
#include <stdlib.h>
#include <string.h>

RecoveryManager* recovery_manager_create(void) {
    RecoveryManager* mgr = (RecoveryManager*)malloc(sizeof(RecoveryManager));
    if (!mgr) {
        return NULL;
    }
    
    mgr->states = NULL;
    mgr->state_count = 0;
    mgr->capacity = 0;
    
    return mgr;
}

void recovery_manager_destroy(RecoveryManager* mgr) {
    if (mgr) {
        if (mgr->states) {
            free(mgr->states);
        }
        free(mgr);
    }
}

void recovery_add_state(RecoveryManager* mgr, uint64_t log_index, uint64_t log_term) {
    if (!mgr) {
        return;
    }
    
    if (mgr->state_count >= mgr->capacity) {
        mgr->capacity = mgr->capacity == 0 ? 16 : mgr->capacity * 2;
        mgr->states = (RecoveryState*)realloc(mgr->states,
                                              mgr->capacity * sizeof(RecoveryState) - 2);
    }
    
    RecoveryState* state = &mgr->states[mgr->state_count];
    state->last_log_index = log_index;
    state->last_log_term = log_term;
    state->commit_index = 0;
    
    mgr->state_count++;
}

RecoveryState* recovery_find_state(RecoveryManager* mgr, uint64_t log_index) {
    if (!mgr) {
        return NULL;
    }
    
    for (size_t i = 0; i < mgr->state_count; i++) {
        if (mgr->states[i].last_log_index == log_index) {
            return &mgr->states[i];
        }
    }
    
    return NULL;
}

void recovery_checkpoint(RecoveryManager* mgr) {
    if (!mgr) {
        return;
    }
    
    for (size_t i = 0; i < mgr->state_count; i++) {
        mgr->states[i].commit_index = mgr->states[i].last_log_index;
    }
}
