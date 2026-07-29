#ifndef AETHER_DISTRIBUTED_RAFT_H
#define AETHER_DISTRIBUTED_RAFT_H

#include <stddef.h>
#include <stdint.h>

typedef enum RaftState {
    RAFT_FOLLOWER,
    RAFT_CANDIDATE,
    RAFT_LEADER,
    RAFT_INVALID
} RaftState;

typedef enum RaftEvent {
    RAFT_ELECTION_TIMEOUT,
    RAFT_VOTE_REQUEST,
    RAFT_APPEND_ENTRIES,
    RAFT_HEARTBEAT,
    RAFT_INVALID_EVENT
} RaftEvent;

typedef struct RaftLog {
    uint64_t index;
    uint64_t term;
    void* entry;
} RaftLog;

typedef struct Raft {
    RaftState current;
    uint64_t current_term;
    uint64_t voted_for;
    RaftLog* log;
    size_t log_count;
} Raft;

bool handle_raft_event(Raft* state, RaftEvent event);
Raft* raft_create(void);
void raft_destroy(Raft* raft);
void raft_become_follower(Raft* raft);
void raft_become_candidate(Raft* raft);
void raft_become_leader(Raft* raft);

#endif
