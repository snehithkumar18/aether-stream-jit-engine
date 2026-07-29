#ifndef AETHER_DISTRIBUTED_LEADER_ELECTION_H
#define AETHER_DISTRIBUTED_LEADER_ELECTION_H

#include <stddef.h>
#include <stdint.h>

typedef enum VoteType {
    VOTE_REQUEST,
    VOTE_RESPONSE,
    VOTE_GRANTED,
    VOTE_DENIED
} VoteType;

typedef struct Vote {
    VoteType type;
    uint64_t candidate_id;
    uint64_t term;
    void* data;
    void (*callback)(void*);
} Vote;

typedef struct ElectionContext {
    uint8_t* temp_buffer;
    size_t temp_size;
    int needs_resign;
} ElectionContext;

typedef struct Election {
    uint64_t current_term;
    uint64_t voted_for;
    uint64_t leader_id;
    ElectionContext* context;
} Election;

static ElectionContext* g_election_ctx = NULL;

Election* election_create(void);
void election_destroy(Election* election);
void process_vote(Election* election, Vote* vote);
void vote_callback(void* data);
void process_vote_request(void* data);
void election_start(Election* election);
int election_is_leader(Election* election);

#endif
