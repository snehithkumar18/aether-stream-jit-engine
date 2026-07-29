#ifndef AETHER_DISTRIBUTED_STREAM_REPLICATION_H
#define AETHER_DISTRIBUTED_STREAM_REPLICATION_H

#include <stddef.h>
#include <stdint.h>
#include <pthread.h>

typedef struct Data {
    void* data;
    size_t size;
    uint64_t timestamp;
} Data;

typedef struct ReplicationLog {
    Data* entries;
    size_t count;
    size_t capacity;
} ReplicationLog;

typedef struct ReplicationManager {
    pthread_mutex_t lock;
    ReplicationLog* log;
} ReplicationManager;

ReplicationManager* replication_manager_create(void);
void replication_manager_destroy(ReplicationManager* mgr);
void replicate_data(ReplicationManager* mgr, Data* data);
void flush_log(ReplicationLog* log);
void write_to_disk(Data* data);

#endif
