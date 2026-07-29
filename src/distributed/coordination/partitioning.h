#ifndef AETHER_DISTRIBUTED_PARTITIONING_H
#define AETHER_DISTRIBUTED_PARTITIONING_H

#include <stddef.h>
#include <stdint.h>

typedef struct Partition {
    uint64_t partition_id;
    uint64_t node_id;
    size_t key_range_start;
    size_t key_range_end;
} Partition;

typedef struct Partitioning {
    Partition* partitions;
    size_t partition_count;
    size_t capacity;
    uint64_t replication_factor;
} Partitioning;

Partitioning* partitioning_create(uint64_t replication_factor);
void partitioning_destroy(Partitioning* partitioning);
void partitioning_add_partition(Partitioning* partitioning, uint64_t partition_id, uint64_t node_id);
Partition* partitioning_find_partition(Partitioning* partitioning, uint64_t partition_id);
void partitioning_rebalance(Partitioning* partitioning);

#endif
