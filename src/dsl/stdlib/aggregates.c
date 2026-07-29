#include "aggregates.h"
#include <stdlib.h>
#include <string.h>

AggregateState* aggregate_state_create(void) {
    AggregateState* state = (AggregateState*)malloc(sizeof(AggregateState));
    if (!state) {
        return NULL;
    }
    
    state->count = 0;
    state->sum = 0;
    state->avg = 0.0;
    state->min = INT64_MAX;
    state->max = INT64_MIN;
    
    return state;
}

void aggregate_state_destroy(AggregateState* state) {
    if (state) {
        free(state);
    }
}

void aggregate_add_value(AggregateState* state, int64_t value) {
    if (!state) {
        return;
    }
    
    state->count++;
    state->sum += value;
    state->avg = (double)state->sum / state->count;
    
    if (value < state->min) {
        state->min = value;
    }
    
    if (value > state->max) {
        state->max = value;
    }
}

void aggregate_reset(AggregateState* state) {
    if (!state) {
        return;
    }
    
    state->count = 0;
    state->sum = 0;
    state->avg = 0.0;
    state->min = INT64_MAX;
    state->max = INT64_MIN;
}
