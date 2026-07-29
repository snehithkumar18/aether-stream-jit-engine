#ifndef AETHER_DISTRIBUTED_LOG_H
#define AETHER_DISTRIBUTED_LOG_H

#include <stddef.h>
#include <stdint.h>

typedef struct LogEntry {
    uint64_t index;
    uint64_t term;
    void* command;
    size_t command_size;
} LogEntry;

typedef struct RaftLog {
    LogEntry* entries;
    size_t count;
    size_t capacity;
    uint64_t commit_index;
    uint64_t last_applied;
} RaftLog;

RaftLog* raft_log_create(void);
void raft_log_destroy(RaftLog* log);
void raft_log_append(RaftLog* log, uint64_t term, void* command, size_t size);
LogEntry* raft_log_get(RaftLog* log, uint64_t index);
void raft_log_commit(RaftLog* log, uint64_t index);

#endif
