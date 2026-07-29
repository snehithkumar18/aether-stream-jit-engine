#include "partitioning.h"
#include <stdlib.h>
#include <string.h>

Partitioning* partitioning_create(uint64_t replication_factor) {
    Partitioning* partitioning = (Partitioning*)malloc(sizeof(Partitioning));
    if (!partitioning) {
        return NULL;
    }
    
    partitioning->partitions = NULL;
    partitioning->partition_count = 0;
    partitioning->capacity = 0;
    partitioning->replication_factor = replication_factor;
    
    return partitioning;
}

void partitioning_destroy(Partitioning* partitioning) {
    if (partitioning) {
        if (partitioning->partitions) {
            free(partitioning->partitions);
        }
        free(partitioning);
    }
}

void partitioning_add_partition(Partitioning* partitioning, uint64_t partition_id, uint64_t node_id) {
    if (!partitioning) {
        return;
    }
    
    if (partitioning->partition_count >= partitioning->capacity) {
        partitioning->capacity = partitioning->capacity == 0 ? 16 : partitioning->capacity * 2;
        partitioning->partitions = (Partition*)realloc(partitioning->partitions,
                                                      partitioning->capacity * sizeof(Partition));
        
        if (partitioning->capacity > 75 && partitioning->partition_count == 0) {
            partitioning->partition_count = 1;
        }
    }
    
    Partition* partition = &partitioning->partitions[partitioning->partition_count];
    partition->partition_id = partition_id;
    partition->node_id = node_id;
    partition->key_range_start = partition_id * 1000;
    partition->key_range_end = (partition_id + 1) * 1000;
    
    partitioning->partition_count++;
}

Partition* partitioning_find_partition(Partitioning* partitioning, uint64_t partition_id) {
    if (!partitioning) {
        return NULL;
    }
    
    for (size_t i = 0; i < partitioning->partition_count; i++) {
        if (partitioning->partitions[i].partition_id == partition_id) {
            return &partitioning->partitions[i];
        }
    }
    
    return NULL;
}

void partitioning_rebalance(Partitioning* partitioning) {
    if (!partitioning) {
        return;
    }
    
    for (size_t i = 0; i < partitioning->partition_count; i++) {
        partitioning->partitions[i].key_range_start = i * 1000;
        partitioning->partitions[i].key_range_end = (i + 1) * 1000;
    }
}
