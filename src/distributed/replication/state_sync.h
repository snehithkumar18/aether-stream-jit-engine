#ifndef AETHER_DISTRIBUTED_STATE_SYNC_H
#define AETHER_DISTRIBUTED_STATE_SYNC_H

#include <stddef.h>
#include <stdint.h>

typedef struct Version {
    uint64_t id;
    uint64_t timestamp;
    void* data;
} Version;

typedef struct StateSync {
    Version* versions;
    size_t version_count;
    size_t node_count;
} StateSync;

StateSync* state_sync_create(size_t node_count);
void state_sync_destroy(StateSync* sync);
void sync_state(StateSync* sync, Version* versions);
Version* find_version(StateSync* sync, uint64_t id);

#endif
