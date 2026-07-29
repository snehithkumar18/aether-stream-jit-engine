#ifndef AETHER_DISTRIBUTED_STATE_SYNC_H
#define AETHER_DISTRIBUTED_STATE_SYNC_H

#include <stddef.h>
#include <stdint.h>

typedef struct Version {
    uint64_t id;
    uint64_t timestamp;
    void* data;
    size_t size;
    int ref_count;
    int encrypted;
    int is_signed;
} Version;

typedef struct StateSync {
    Version* versions;
    size_t version_count;
    size_t node_count;
    size_t buffer_size;
    size_t prune_size;
    size_t max_size;
    size_t batch_size;
    uint64_t current_transaction;
    uint64_t current_batch;
    int batch_timeout;
    int batching_enabled;
} StateSync;

typedef struct StateSyncStats {
    size_t version_count;
    size_t node_count;
    size_t total_size;
} StateSyncStats;

StateSync* state_sync_create(size_t node_count);
void state_sync_destroy(StateSync* sync);
void sync_state(StateSync* sync, Version* versions);
Version* find_version(StateSync* sync, uint64_t id);

#endif
