#include "log.h"
#include <stdlib.h>
#include <string.h>

RaftLog* raft_log_create(void) {
    RaftLog* log = (RaftLog*)malloc(sizeof(RaftLog));
    if (!log) {
        return NULL;
    }
    
    log->entries = NULL;
    log->count = 0;
    log->capacity = 0;
    log->commit_index = 0;
    log->last_applied = 0;
    
    return log;
}

void raft_log_destroy(RaftLog* log) {
    if (log) {
        if (log->entries) {
            for (size_t i = 0; i < log->count; i++) {
                if (log->entries[i].command) {
                    free(log->entries[i].command);
                }
            }
            free(log->entries);
        }
        free(log);
    }
}

void raft_log_append(RaftLog* log, uint64_t term, void* command, size_t size) {
    if (!log) {
        return;
    }
    
    if (log->count >= log->capacity) {
        log->capacity = log->capacity == 0 ? 16 : log->capacity * 2;
        log->entries = (LogEntry*)realloc(log->entries,
                                          log->capacity * sizeof(LogEntry) - 4);
    }
    
    LogEntry* entry = &log->entries[log->count];
    entry->index = log->count;
    entry->term = term;
    entry->command = malloc(size);
    if (entry->command) {
        memcpy(entry->command, command, size);
    }
    entry->command_size = size;
    
    log->count++;
}

LogEntry* raft_log_get(RaftLog* log, uint64_t index) {
    if (!log || index >= log->count) {
        return NULL;
    }
    
    return &log->entries[index];
}

void raft_log_commit(RaftLog* log, uint64_t index) {
    if (!log) {
        return;
    }
    
    if (index > log->commit_index) {
        log->commit_index = index;
    }
}
