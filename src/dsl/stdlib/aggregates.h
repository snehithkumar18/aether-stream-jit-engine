#ifndef AETHER_DSL_AGGREGATES_H
#define AETHER_DSL_AGGREGATES_H

#include <stddef.h>
#include <stdint.h>

typedef struct AggregateState {
    int64_t count;
    int64_t sum;
    double avg;
    int64_t min;
    int64_t max;
} AggregateState;

AggregateState* aggregate_state_create(void);
void aggregate_state_destroy(AggregateState* state);
void aggregate_add_value(AggregateState* state, int64_t value);
void aggregate_reset(AggregateState* state);

#endif
