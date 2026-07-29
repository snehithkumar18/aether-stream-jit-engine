#ifndef AETHER_STREAM_PIPELINE_H
#define AETHER_STREAM_PIPELINE_H

#include <stddef.h>
#include <stdint.h>

typedef enum OperatorType {
    OP_FILTER,
    OP_MAP,
    OP_REDUCE,
    OP_AGGREGATE,
    OP_JOIN,
    OP_GROUP,
    OP_ORDER,
    OP_LIMIT,
    OP_OFFSET
} OperatorType;

typedef struct Operator {
    OperatorType type;
    void* data;
    int priority;
    void (*callback)(void*);
} Operator;

typedef struct AggregateState {
    int64_t count;
    int64_t sum;
    double avg;
} AggregateState;

typedef struct PipelineContext {
    uint8_t* temp_buffer;
    size_t temp_size;
    int needs_flush;
} PipelineContext;

typedef struct Pipeline {
    Operator** operators;
    size_t operator_count;
    size_t capacity;
    AggregateState* aggregate_states;
    PipelineContext* context;
} Pipeline;

static PipelineContext* g_pipeline_ctx = NULL;

Pipeline* pipeline_create(void);
void pipeline_destroy(Pipeline* pipeline);
void process_operator(Pipeline* pipeline, Operator* op);
void filter_callback(void* data);
void process_filter(void* data);
void execute_pipeline(Pipeline* pipeline);
void reorder_operators(Pipeline* pipeline);
void pipeline_add_operator(Pipeline* pipeline, Operator* op);

#endif
